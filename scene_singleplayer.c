#include "scene_singleplayer.h"
#include "network.h"
//jogar voltar numero 

Scene_Singleplayer* SceneSingleplayer_new() {
    if(Network.sockFd == 0) {
        Network.sockFd = Socket_Open(0, false);
    }

    Scene_Singleplayer* newScene = malloc(sizeof(Scene_Singleplayer));
    newScene->iniciar = WD_CreateTexture();
    newScene->continuar = WD_CreateTexture();
    newScene->voltar = WD_CreateTexture();
    newScene->background = WD_CreateTexture();
    newScene->mensagem = WD_CreateTexture();
    newScene->mensagem1 = WD_CreateTexture();
    newScene->sim = WD_CreateTexture();
    newScene->nao = WD_CreateTexture();
    newScene->bemVindo = WD_CreateTexture();
    newScene->nome = WD_CreateTexture();
    newScene->pontu = WD_CreateTexture();
    newScene->nivel = WD_CreateTexture();
    newScene->index = 0;
    newScene->primeiraTela = true;
    newScene->waitingConnection = false;
    newScene->temArquivo = true;

    // Arquivo com pontuação salva
    FILE* arq;
    arq = fopen("save.dat", "rb");
    if(arq == NULL)
        newScene->temArquivo=false;
    else {
        fread(&newScene->pontuacao, sizeof(int), 1, arq);
    }

    srand(time(NULL));   // Initialization, should only be called once.
    int r = rand() % 4;
    newScene->posBoneco.x = 256*r; //= {30, 0, 256, 256};
    newScene->posBoneco.y = 0;
    newScene->posBoneco.w = 256;
    newScene->posBoneco.h = 256;


    SDL_Color orange = { 255,172,65 };
    SDL_Color gray = {60, 60, 60};
    SDL_Color white = {255, 255, 255};
    SDL_Color blue  = { 91,116,212 };
    char pontac[15], nivel[15];
    sprintf(pontac, "%d/100", newScene->pontuacao%100);
    sprintf(nivel, "NIVEL %d", newScene->pontuacao/100);

    WD_TextureLoadFromText(newScene->iniciar, "INICIAR", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromText(newScene->continuar, "CONTINUAR", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromText(newScene->voltar, "VOLTAR", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromText(newScene->mensagem, "ISSO IRÁ SOBRESCREVER SEU PROGRESSO,", Game.roboto, gray);
    WD_TextureLoadFromText(newScene->mensagem1, "DESEJA CONTINUAR?", Game.roboto, gray);
    WD_TextureLoadFromText(newScene->sim, "[SIM]", Game.roboto, gray);
    WD_TextureLoadFromText(newScene->nao, "[NÃO]", Game.roboto, gray);
    WD_TextureLoadFromText(newScene->bemVindo, "BEM VINDO", Game.mainMenu_botoes, white );
    WD_TextureLoadFromText(newScene->nome, Game.nome, Game.mainMenu_botoes, blue);
    if(newScene->pontuacao%100 > 50)
        WD_TextureLoadFromText(newScene->pontu, pontac, Game.roboto, orange);
    else
        WD_TextureLoadFromText(newScene->pontu, pontac, Game.roboto, white);
    WD_TextureLoadFromText(newScene->nivel, nivel, Game.roboto, white);

    newScene->imgBoneco = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->imgBoneco, "content/cabecas.png");



    WD_TextureLoadFromFile(newScene->background, "content/bgsingle.png");
    int w = newScene->background->w, h = newScene->background->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w;
    newScene->renderQuad.h = h;

    return newScene;
}

void SceneSingleplayer_openServer(Scene_Singleplayer* s) {
    Network.serverHost = true;
    Network.server = Server_Open(SERVER_DEFAULT_PORT, "Sala", true);
    if (Network.server != NULL) {
        Network.serverThread = SDL_CreateThread(Server_InitLoop, "ServerLoop", Network.server);
        if (Network.serverAddress != NULL)
            DestroyAddress(Network.serverAddress);
        struct sockaddr_in sin;
        int len = sizeof(sin);
        unsigned short port;
        getsockname(Network.server->sockfd, (struct sockaddr *)&sin, &len);
        port = ntohs(sin.sin_port);
        Network.serverAddress = NewAddress(127, 0, 0, 1, port);
        char data[32];
        sprintf(data, "CON 1 1 %s", Game.nome);
        Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
        s->waitingConnection = true;
    }
}

void SceneSingleplayer_update(Scene_Singleplayer* s) {
    if(s->waitingConnection && !SceneManager.inTransition) {
        Address sender;
        char data[32];
        int bytes = Socket_Receive(Network.sockFd, &sender, data, sizeof(data));
        if(bytes > 0 && sender.address == Network.serverAddress->address && sender.port == Network.serverAddress->port) {
            if(strncmp("CON", data, 3) == 0) {
                sscanf(data + 4, "%d", &Network.clientId);
                Network.connectedToServer = true;
                Network.singleplayer = true;
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOBBY);
            }
        }
    }
    SDL_RenderClear(Game.renderer);

    WD_TextureRenderDest(s->background, &s->renderQuad);

    if(s->primeiraTela) {

        if(s->temArquivo) {
            SDL_Rect barraTotal = { 30+256+30, 30+s->nome->h, 500, 80};
            SDL_Rect barraPontuacao = { barraTotal.x, barraTotal.y, (s->pontuacao % 100) / 100.0 * barraTotal.w, barraTotal.h };
            SDL_RenderFillRect(Game.renderer, &barraTotal);
            SDL_SetRenderDrawColor(Game.renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(Game.renderer, &barraPontuacao);

            WD_TextureRender(s->pontu, barraTotal.x , barraTotal.y + barraTotal.h );
            WD_TextureRender(s->nome, barraTotal.x, 30);
            WD_TextureRender(s->nivel, (barraTotal.x - s->nivel->w)/2 , 30+256+30);

            WD_TextureRenderEx(s->imgBoneco, 30, 30, &s->posBoneco, 0.0, NULL, SDL_FLIP_NONE);
        }

        WD_TextureRender(s->iniciar, (REFERENCE_WIDTH - s->iniciar->w)/2, 600);
        WD_TextureRender(s->continuar, (REFERENCE_WIDTH - s->continuar->w)/2, 700);
        WD_TextureRender(s->voltar, (REFERENCE_WIDTH - s->voltar->w)/2, 800);
        //WD_TextureRender(s->bemVindo, 300, 300);

        SDL_Rect iniciar = { (REFERENCE_WIDTH - s->iniciar->w)/2, 600 + s->iniciar->h , s->iniciar->w, 5 };
        SDL_Rect continuar = { (REFERENCE_WIDTH - s->continuar->w)/2, 700 + s->continuar->h , s->continuar->w, 5 };
        SDL_Rect voltar = { (REFERENCE_WIDTH - s->voltar->w)/2, 800 + s->voltar->h , s->voltar ->w, 5 };


        SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 100);

        if(s->index == 0) {
            SDL_RenderFillRect(Game.renderer, &iniciar);
        } else if(s->index == 1) {
            SDL_RenderFillRect(Game.renderer, &continuar); 
        } else if(s->index == 2) {
            SDL_RenderFillRect(Game.renderer, &voltar);
        }
    } else {
        SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 100);
        SDL_Rect boxF = { (REFERENCE_WIDTH - 800)/2,  (REFERENCE_HEIGHT - 400)/2, 800, 400};
        SDL_RenderFillRect(Game.renderer, &boxF);

        WD_TextureRender(s->mensagem, (REFERENCE_WIDTH - 800)/2 + 30, (REFERENCE_HEIGHT - 400)/2 + 50);
        WD_TextureRender(s->mensagem1, (REFERENCE_WIDTH - s->mensagem1->w)/2, (REFERENCE_HEIGHT - 400)/2 + 50 + s->mensagem->h + 3);
        WD_TextureRender(s->sim, (REFERENCE_WIDTH - 800)/2 + 300 - s->sim->w, (REFERENCE_HEIGHT - 400)/2 + 260);
        WD_TextureRender(s->nao, (REFERENCE_WIDTH - 800)/2 + 500, (REFERENCE_HEIGHT - 400)/2 + 260);
        
        SDL_Rect sim = { (REFERENCE_WIDTH - 800)/2 + 300 - s->sim->w, (REFERENCE_HEIGHT - 400)/2 + 260 + s->sim->h, s->sim->w, 5};
        SDL_Rect nao = { (REFERENCE_WIDTH - 800)/2 + 500, (REFERENCE_HEIGHT - 400)/2 + 260 + s->nao->h, s->nao->w, 5};
        if(s->index == 0)
            SDL_RenderFillRect(Game.renderer, &sim);
        else
            SDL_RenderFillRect(Game.renderer, &nao);
    }

}

void SceneSingleplayer_destroy(Scene_Singleplayer* s) {
    WD_TextureDestroy(s->iniciar);
    WD_TextureDestroy(s->continuar);
    WD_TextureDestroy(s->voltar);
    WD_TextureDestroy(s->background);
    WD_TextureDestroy(s->mensagem);
    WD_TextureDestroy(s->sim);
    WD_TextureDestroy(s->nao);
    WD_TextureDestroy(s->bemVindo);
    WD_TextureDestroy(s->nome);
    WD_TextureDestroy(s->pontu);
    free(s);
}

void SceneSingleplayer_handleEvent(Scene_Singleplayer* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(s->primeiraTela) {
            if(e->key.keysym.sym == SDLK_ESCAPE) {
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
            } else if(e->key.keysym.sym == SDLK_UP) {
                if(s->index > 0)
                    s->index--;
            } else if(e->key.keysym.sym == SDLK_DOWN) {
                if(s->index < 2 )
                    s->index++;
            } else if(e->key.keysym.sym == SDLK_RETURN) {
                if(s->index == 0) {
                    if(s->temArquivo)
                        s->primeiraTela = false;
                    else
                        SceneSingleplayer_openServer(s);
                } else if(s->index == 1 ){
                    SceneSingleplayer_openServer(s);
                } else {
                    SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
                }
            }
        } else {
            if(e->key.keysym.sym == SDLK_RIGHT || e->key.keysym.sym == SDLK_LEFT) {
                s->index = !s->index;
            } else if(e->key.keysym.sym == SDLK_RETURN) {
                if(s->index == 0) {
                    SceneSingleplayer_openServer(s);
                    Game.reset = true;
                } else {
                    s->primeiraTela = true;
                }
            }
        }
    }
}
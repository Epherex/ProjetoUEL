#include "scene_singleplayer.h"
#include "network.h"
#include <arpa/inet.h>

Scene_Singleplayer* SceneSingleplayer_new() {
    Scene_Singleplayer* newScene = malloc(sizeof(Scene_Singleplayer));
    SDL_Color color = {0, 255, 0};
    newScene->textTexture = WD_CreateTexture();
    WD_TextureLoadFromText(newScene->textTexture, "Uhulll", gInfo.mainFont, color);
    newScene->testTexture = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->testTexture, "img.png");
    newScene->renderQuad.x = (SCREEN_WIDTH - newScene->textTexture->w) / 2;
    newScene->renderQuad.y = (SCREEN_HEIGHT - newScene->textTexture->h) / 2;
    newScene->cont = 0;
    newScene->socketFd = Socket_Open(3000);
    newScene->s2 = Socket_Open(0);
    return newScene;
}

void SceneSingleplayer_update(Scene_Singleplayer* s) {
    Address* sender = malloc(sizeof(Address));
    char data[256];
    if(Socket_Receive(s->socketFd, sender, data, sizeof(data)) > 0) {
        char salve[256];
        struct in_addr oi;
        oi.s_addr = htonl(sender->address);
        inet_ntop(AF_INET, &oi, salve, sizeof(salve));
        printf("%s:%u\n", salve, sender->port);
        printf("%s\n", data);
    }
    free(sender);
    SDL_SetRenderDrawColor(gInfo.renderer, 0x34, 0x92, 0x22, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    WD_TextureRender(s->textTexture, s->renderQuad.x, s->renderQuad.y);
    WD_TextureRender(s->testTexture, 15 * s->cont, 100);
    s->cont++;
    if(s->cont > 60) {
        s->cont = 0;
    }
}

void SceneSingleplayer_destroy(Scene_Singleplayer* s) {
    Socket_Close(s->socketFd);
    Socket_Close(s->s2);
    WD_TextureDestroy(s->textTexture);
    WD_TextureDestroy(s->testTexture);
    free(s);
}

void SceneSingleplayer_handleEvent(SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        } else if(e->key.keysym.sym == SDLK_TAB) {
            Address* ad = NewAddress(191, 52, 64, 165, 3000);
            printf("%d\n", ad->address);
            char data[] = "Testando";
            Socket_Send(sMng.sSingleplayer->s2, ad, data, sizeof(data));
            free(ad);
        }
    }
}
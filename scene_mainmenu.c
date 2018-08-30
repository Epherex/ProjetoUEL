#include "scene_mainmenu.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    //SDL_Surface* textSurface = TTF_RenderUTF8_Blended(gInfo.mainFont, "Testesao", color);
    
    SDL_Surface * textSurface = IMG_Load("BG_Login.png");//BACKGROUND LOGIN

    SDL_Color colorLogar = {255, 156, 0};//CORES INICIAIS DOS BOTÕES
    SDL_Color colorOff = {255,255,255};

    newScene->textTexture = WD_CreateTexture();
    newScene->textLogar = WD_CreateTexture();

    WD_TextureLoadFromText(newScene->textLogar, "Logar" , gInfo.menuFont, colorLogar);

    newScene->textTexture = SDL_CreateTextureFromSurface(gInfo.renderer, textSurface);
    int w = textSurface->w, h = textSurface->h;
    newScene->renderQuad.x =0;// (SCREEN_WIDTH - w) / 2;
    newScene->renderQuad.y =0;// (SCREEN_HEIGHT - h) / 2;
    newScene->renderQuad.w = w/2;
    newScene->renderQuad.h = h/2;
    return newScene;
}

void SceneMainMenu_update(Scene_MainMenu* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    SDL_RenderCopy(gInfo.renderer, s->textTexture, NULL, &s->renderQuad);
    WD_TextureRender(s->textLogar, 325/2, 800/2);

}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    SDL_DestroyTexture(s->textTexture);
    SDL_DestroyTexture(s->textLogar);
    free(s);
}

void SceneMainMenu_handleEvent(SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SINGLEPLAYER);
        }
    }
}
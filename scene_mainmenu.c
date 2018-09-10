#include "scene_mainmenu.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->bemvindo = WD_CreateTexture();
    newScene->nome = WD_CreateTexture();
    newScene->jogar = WD_CreateTexture();
    newScene->tutorial = WD_CreateTexture();
    newScene->logout = WD_CreateTexture();
    newScene->jogarOff = WD_CreateTexture();
    newScene->tutorialOff = WD_CreateTexture();
    newScene->logoutOff = WD_CreateTexture();
    newScene->index = 0;

    SDL_Color colorBemvindo = {141,38,38}; 
    SDL_Color colorNome = {255, 255, 255};
    SDL_Color colorSelected = {255, 66, 0};
    SDL_Color colorNotSelected = {255, 255, 255};

    WD_TextureLoadFromText(newScene->bemvindo,"Bem Vindo", gInfo.mainMenu, colorBemvindo);
    WD_TextureLoadFromText(newScene->nome, "Basto Forte", gInfo.mainMenu, colorNome);

    WD_TextureLoadFromText(newScene->jogar, "Jogar", gInfo.mainMenu_botoes, colorSelected);
    WD_TextureLoadFromText(newScene->tutorial, "Tutorial", gInfo.mainMenu_botoes, colorSelected);
    WD_TextureLoadFromText(newScene->logout, "Logout", gInfo.mainMenu_botoes, colorSelected);
    WD_TextureLoadFromText(newScene->jogarOff, "Jogar", gInfo.mainMenu_botoes, colorNotSelected);
    WD_TextureLoadFromText(newScene->tutorialOff, "Tutorial", gInfo.mainMenu_botoes, colorNotSelected);
    WD_TextureLoadFromText(newScene->logoutOff, "Logout", gInfo.mainMenu_botoes, colorNotSelected);

    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_mainMenu.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w*gInfo.screenMulti;
    newScene->renderQuad.h = h*gInfo.screenMulti;

    return newScene;
}



void SceneMainMenu_update(Scene_MainMenu* s) {
    //SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    WD_TextureRender(s->bemvindo, 84 * gInfo.screenMulti, 566 * gInfo.screenMulti);
    WD_TextureRender(s->nome, 84 * gInfo.screenMulti, 634 * gInfo.screenMulti);
    //WD_TextureRender(s->jogar, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
    //WD_TextureRender(s->tutorial, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
    //WD_TextureRender(s->logout, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);

    if(s->index == 0) {
        WD_TextureRender(s->jogar, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
        WD_TextureRender(s->tutorialOff, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
        WD_TextureRender(s->logoutOff, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);
    } else if(s->index == 1) {
        WD_TextureRender(s->jogarOff, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
        WD_TextureRender(s->tutorial, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
        WD_TextureRender(s->logoutOff, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);
    } else {
        WD_TextureRender(s->jogarOff, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
        WD_TextureRender(s->tutorialOff, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
        WD_TextureRender(s->logout, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);
    }

}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->bemvindo);
    WD_TextureDestroy(s->nome);
    WD_TextureDestroy(s->jogar);
    WD_TextureDestroy(s->tutorial);
    WD_TextureDestroy(s->logout);
    WD_TextureDestroy(s->jogarOff);
    WD_TextureDestroy(s->tutorialOff);
    WD_TextureDestroy(s->logoutOff);
    free(s);
    
}

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e) {

    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if((e->key.keysym.sym == SDLK_DOWN ) && s->index < 2) {
            s->index++;
        } else if(e->key.keysym.sym == SDLK_UP && s->index > 0) {
            s->index--;
        }
    }
    

}
// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_STB_ASSET_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKENDS_STB_ASSET_PROVIDER_H

#include "engine/backend/asset_provider.h"

//TODO: use stb_rect_pack and other tech to automatically build sprite atlases
namespace backend {
    class StbAssetProvider : public IAssetProvider {
    public:
        StbAssetProvider();

        Audio loadAudio(const char* path) override;
        void unloadAudio(Audio audio) override;

        Image loadImage(const char* path) override;
        void unloadImage(Image image) override;

        Font loadFont(const char* path, int size, const unicode::codepoint* codepoints, int codepointCount) override;
        void unloadFont(Font font) override;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_STB_ASSET_PROVIDER_H

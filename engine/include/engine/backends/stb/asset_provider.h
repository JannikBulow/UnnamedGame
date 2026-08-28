// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_STB_ASSET_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKENDS_STB_ASSET_PROVIDER_H

#include "engine/backend/asset_provider.h"

namespace backend {
    class StbAssetProvider : public IAssetProvider {
    public:
        Image loadImage(const char* path) override;
        void unloadImage(Image image) override;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_STB_ASSET_PROVIDER_H

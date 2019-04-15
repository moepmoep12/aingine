#pragma once

#include <functional>
#include <string>
#include <typeindex>

#include "Rendering/bitmap.h"

namespace AIngine::Assets {

	using UID = size_t;

	extern UID GetUID(std::string const & path);

	class AssetBase
	{

	public:
		const UID uid;
		const std::string path;
		virtual ~AssetBase() {};

	protected:
		AssetBase(std::string const & path) :
			path(path),
			uid(GetUID(path)) {}

	};

	class AssetFactory
	{
		friend class AssetRegistry;
	protected:
		virtual std::unique_ptr<AssetBase> LoadAsset(std::string const & path) = 0;

	public:
		virtual ~AssetFactory() {};
	};

	class AssetRegistry
	{
		using AssetMap = std::unordered_map<UID, std::unique_ptr<AssetBase>>;
		using FactoryMap = std::unordered_map<std::type_index, std::unique_ptr<AssetFactory>>;

	public:

		void RegisterFactory(std::type_index type, std::unique_ptr<AssetFactory> factory)
		{
			factories[type] = std::move(factory);
		}

		template<typename T>
		void RegisterFactory(std::unique_ptr<AssetFactory> factory)
		{
			factories[std::type_index(typeid(T))] = std::move(factory);
		}

		AssetBase * Get(UID uid) const
		{
			return assets.at(uid).get();
		}

		AssetBase * Load(std::string const & path, std::type_index type)
		{
			UID uid = GetUID(path);
			auto & iter = assets.find(uid);
			if (iter != assets.end())
				return iter->second.get();

			assets[uid] = std::move(factories.at(type)->LoadAsset(path));

			return assets[uid].get();
		}

		template<typename T>
		T * Load(std::string const & path)
		{
			return (T*)Load(path, std::type_index(typeid(T)));
		}

		void Shutdown() {
			assets.clear();
			factories.clear();
		}

	private:
		AssetMap assets;
		FactoryMap factories;
	};


	class BitmapAsset : public AssetBase
	{
		friend class BitmapAssetFactory;

	public:
		Bitmap & GetBitmap() {
			return bitmap;
		}
		virtual ~BitmapAsset() {};

	private:
		Bitmap bitmap;

	protected:
		BitmapAsset(std::string const & path) :
			AssetBase(path),
			bitmap(path) {}

	};

	class BitmapAssetFactory : public AssetFactory 
	{
		friend class AssetRegistry;

		virtual ~BitmapAssetFactory() {}

	protected:

		virtual std::unique_ptr<AssetBase> LoadAsset(std::string const & path) 
		{
			return std::unique_ptr<BitmapAsset>(new BitmapAsset(path));
		};
	};
}
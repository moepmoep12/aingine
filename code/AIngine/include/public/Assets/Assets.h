#pragma once

#include <functional>
#include <string>
#include <typeindex>
#include <vector>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <filesystem>

#include "Rendering/bitmap.h"
#include "Rendering/shader.h"
#include "cute_sound.h"
#include "Rendering/Font.h"

namespace AIngine {
	class Application;
}

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


	/******************************** BitMap *********************************************/

	class BitmapAsset final : public AssetBase
	{
		friend class BitmapAssetFactory;

	public:
		AIngine::Rendering::Bitmap & GetBitmap() {
			return bitmap;
		}
		virtual ~BitmapAsset() {};

	private:
		AIngine::Rendering::Bitmap bitmap;

	protected:
		BitmapAsset(std::string const & path) :
			AssetBase(path),
			bitmap(path) {}

	};

	class BitmapAssetFactory final : public AssetFactory
	{
		friend class AssetRegistry;

	public:
		virtual ~BitmapAssetFactory() {}

	protected:

		virtual std::unique_ptr<AssetBase> LoadAsset(std::string const & path) override
		{
			return std::unique_ptr<BitmapAsset>(new BitmapAsset(std::filesystem::canonical(path).string()));
		};
	};


	/********************************** ShaderAsset ******************************************/

	inline std::string ShaderPath(const std::string& vertexPath, const std::string& fragmentPath)
	{
		std::stringstream ss;
		ss << '\n';
		ss << vertexPath << '\n';
		ss << fragmentPath << '\n';
		return ss.str();
	}

	class ShaderAsset final : public AssetBase
	{
		friend class ShaderAssetFactory;

	public:
		AIngine::Rendering::GLShaderProgram& GetShader() {
			return m_shader;
		}

		virtual ~ShaderAsset() {};

	private:
		AIngine::Rendering::GLShaderProgram m_shader;

		inline std::string& GetPath(std::string const &vertexPath, std::string const &fragmentPath) const {
			static std::string result;
			result.append(vertexPath);
			result.append(";");
			result.append(fragmentPath);
			return result;
		}

	protected:
		ShaderAsset(std::string const &vertexPath, std::string const &fragmentPath)
			: AssetBase(GetPath(vertexPath, fragmentPath)), m_shader(vertexPath, fragmentPath) {}
	};

	class ShaderAssetFactory final : public AssetFactory {

		friend class AssetRegistry;

	public:
		virtual ~ShaderAssetFactory() {}

	protected:
		virtual std::unique_ptr<AssetBase> LoadAsset(std::string const & path) override
		{
			std::vector<std::string> paths = SplitPath(path);
			return std::unique_ptr<ShaderAsset>(new ShaderAsset(paths[0], paths[1]));
		}

		//void split(const std::string& s, char c,
		//	std::vector<std::string>& v) {
		//	std::string::size_type i = 0;
		//	std::string::size_type j = s.find(c);

		//	while (j != std::string::npos) {
		//		v.push_back(s.substr(i, j - i));
		//		i = ++j;
		//		j = s.find(c, j);

		//		if (j == std::string::npos)
		//			v.push_back(s.substr(i, s.length()));
		//	}
		//}

		std::vector<std::string> SplitPath(const std::string& path)
		{
			std::vector<std::string> result;
			std::istringstream s(path);
			unsigned int i = 0;
			std::string resourceFolderPath;
			for (std::string line; std::getline(s, line); )
			{
				switch (i) {
				case 0:
					resourceFolderPath = line;
					resourceFolderPath.erase(std::remove(resourceFolderPath.begin(), resourceFolderPath.end(), '\n'), resourceFolderPath.end());
					break;
				case 1:
					//std::replace(line.begin(), line.end(), '/', '\\');
					result.push_back(std::filesystem::canonical(resourceFolderPath + line).string());
					break;
				case 2:
					//std::replace(line.begin(), line.end(), '/', '\\');
					result.push_back(std::filesystem::canonical(resourceFolderPath + line).string());
					break;
				}
				i++;
			}
			return result;
		}
	};

	/********************************** SoundAsset ******************************************/
	class SoundAsset final :public AssetBase
	{
		friend class SoundAssetFactory;

	public:
		inline cs_loaded_sound_t& GetLoadedSound() {
			return m_loadedSound;
		}

		virtual ~SoundAsset() {
			cs_free_sound(&m_loadedSound);
		};

	private:
		cs_loaded_sound_t m_loadedSound;

	protected:
		SoundAsset(std::string const &path)
			: AssetBase(path), m_loadedSound(cs_load_wav(path.c_str()))
		{
		}
	};

	class SoundAssetFactory final : public AssetFactory {

		friend class AssetRegistry;

	public:
		virtual ~SoundAssetFactory() {}

	protected:
		virtual std::unique_ptr<AssetBase> LoadAsset(std::string const & path) override
		{
			return std::unique_ptr<SoundAsset>(new SoundAsset(std::filesystem::canonical(path).string()));
		}
	};

	/********************************** FontAsset ******************************************/

	class FontAsset final :public AssetBase
	{
		friend class FontAssetFactory;

	public:
		virtual ~FontAsset() {};
		AIngine::Rendering::Font& GetFont() { return m_font; }

	private:
		AIngine::Rendering::Font m_font;


	protected:
		FontAsset(std::string const &path, unsigned int size)
			: AssetBase(path), m_font(path.c_str(), size) {}
	};

	class FontAssetFactory final : public AssetFactory {

		friend class AssetRegistry;

	public:
		virtual ~FontAssetFactory() {}

	protected:
		virtual std::unique_ptr<AssetBase> LoadAsset(std::string const & path) override
		{
			unsigned int size;
			std::string p;
			GetPathAndSize(path, &size, &p);
			return std::unique_ptr<FontAsset>(new FontAsset(p, size));
		}

	private:
		void GetPathAndSize(const std::string& path, unsigned int* size, std::string* outpath) {
			std::istringstream s(path);
			unsigned int i = 0;
			for (std::string line; std::getline(s, line); )
			{
				switch (i) {
				case 0:
					*outpath = std::filesystem::canonical(line).string();
					break;
				case 1:
					*size = std::stoi(line);
					break;
				}
				i++;
			}

		}
	};


	/********************************** AssetRegistry ******************************************/

	class AssetRegistry final
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

		static AssetBase * Get(UID uid)
		{
			if (s_instance) {
				return s_instance->assets.at(uid).get();
			}
			else return nullptr;
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
		static T * Load(std::string const & path)
		{
			if (s_instance) {
				std::filesystem::path p(path);
				std::string result;

				if (p.is_relative()) {
					// Is it an engine resource?
					if (path.find("AIngine") != std::string::npos) {
						result = s_instance->m_engineInstallPath + "Resources\\" + path;
					}
					else
						result = s_instance->m_resourceFolderPath + path;
				}
				else
					result = path;
				return (T*)s_instance->Load(result, std::type_index(typeid(T)));
			}
			else return nullptr;
		}



	private:
		friend class AIngine::Application;

		AssetRegistry();

		void Shutdown() {
			assets.clear();
			factories.clear();
		}

	private:
		AssetMap assets;
		FactoryMap factories;
		static AssetRegistry* s_instance;
		std::string m_resourceFolderPath;
		std::string m_engineInstallPath;
	};
}
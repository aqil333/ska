#pragma once
#include "Color.h"
#include "../Rectangle.h"

namespace ska {
	class Texture;
	class TextureData;
    class SDLRenderer;

	/**
	* \brief SDL specific, a RAII wrapper of a SDL_Texture instance
	*/
	class SDLTexture {
		friend class Texture;
		friend class SDLRenderer;

	public:
		SDLTexture();
		
		explicit SDLTexture(TextureData& p);
		
		SDLTexture(SDLTexture&) = delete;
		SDLTexture& operator=(const SDLTexture&) = delete;

		void load(const SDLRenderer& renderer, const std::string& id, int r = -1, int g = -1, int b = -1, int a = -1);
		void loadFromColoredRect(const SDLRenderer& renderer, const Color& pair, const Rectangle& rect);
		void loadFromText(const SDLRenderer& renderer, unsigned int fontSize, const std::string& text, Color c);
		void free();
		~SDLTexture();

	private:
		Uint8 m_r, m_g, m_b;

		//La texture wrapp�e
		SDL_Texture* m_texture;

		//Largeur de la texture
		unsigned int m_w;

		//Hauteur de la texture
		unsigned int m_h;

		Uint8 m_alpha;

		//Nom du fichier qui est charg� lors d'un .load
		std::string m_fileName;
	};
}


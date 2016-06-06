#ifndef __SCENE__
# define __SCENE__

# include "RENDERING/Types.h"

# include <string>
# include <vector>

namespace GGE
{
	//
	// Can be a screen (combine with camera/view), a gif-like video, a zelda-like level / map-tile, etc...
	//
	template <typename T_Drawable>
	struct Scene
	{
		using T_DrawableType = std::shared_ptr<T_Drawable>;
		using T_DrawableTypeVector = typename std::vector < T_DrawableType > ;
		using T_Drawable_base = typename T_Drawable;
		using T_DrawableTypeVector_base = typename std::vector < T_Drawable >;

		Scene() = default;
		//Scene(const std::string & backgroundTexturePath, T_DrawableTypeVector && T_DrawableTypeVector = T_DrawableTypeVector())
		Scene(const std::string & backgroundTexturePath, std::initializer_list<T_DrawableType> & T_DrawableTypeVector)
			: _drawables(T_DrawableTypeVector.begin(), T_DrawableTypeVector.end())
		{
			this->LoadBackground(backgroundTexturePath);
		}
		~Scene() = default;

		inline Scene &				operator+=(const T_DrawableTypeVector & drawables)
		{
			this->_drawables.emplace_back(drawables);
			return *this;
		}
		inline Scene &				operator+=(T_DrawableType && drawable)
		{
			this->_drawables.emplace_back(drawable);
			return *this;
		}
		inline Scene &				operator+=(T_DrawableTypeVector_base &&)
		{
			// [Todo]
			assert(false);
		}
		inline T_DrawableTypeVector &	GetContent(void)
		{
			return this->_drawables;
		}

		void						Draw(void)
		{
			assert(_window != 0x0);

			_window->clear();
			_window->draw(this->_backgroundSprite);
			std::for_each(_drawables.begin(), _drawables.end(), [&](T_DrawableType drawable){ drawable->Draw(*_window); });
			_window->display();
		}
		static void					BindWindow(RenderWindow	& renderWindows)
		{
			_window = &renderWindows;
		}

	protected:
		static RenderWindow	*		_window;

		T_DrawableTypeVector			_drawables;
		Sprite						_backgroundSprite;
		Texture						_bufBatckgroundTexture; // To use as buffer. [Todo]=[To_test] -> SetSmooth

		inline void	LoadBackground(const std::string & texture_path)
		{
			if (!(_bufBatckgroundTexture.loadFromFile(texture_path)))
				throw GCL::Exception("[Error] : GGE::Game::SetBackground : Cannot load texture from file : " + texture_path);
			this->_backgroundSprite.setTexture(_bufBatckgroundTexture);
		}
	};
	template <typename T_Drawable>
	RenderWindow	*	Scene<T_Drawable>::_window = 0x0;
}

#endif // __SCENE__
#ifndef __SCENE__
# define __SCENE__

# include "RENDERING/Types.h"

# include <string>
# include <vector>

namespace GGE
{
	template <typename T_Drawable>
	struct Scene // or Screne [?]
	{
		using DrawableVectorType = typename std::vector < T_Drawable > ;

		// Background
		// std::vector<Drawable>

		Scene() = default;
		Scene(const std::string & backgroundTexturePath, DrawableVectorType && DrawableVectorType = DrawableVectorType())
			: _drawables(DrawableVectorType)
		{
			this->LoadBackground(backgroundTexturePath);
		}

		void	Draw(void)
		{
			assert(_window != 0x0);

			_window.clear();
			_window.draw(this->_backgroundSprite);
			_window.display();
		}

		static void	BindWindow(RenderWindow	* renderWindows)
		{
			_window = renderWindows;
		}

	protected:
		static RenderWindow	*	_window;

		DrawableVectorType		_drawables;
		Sprite					_backgroundSprite;
		Texture					_bufBatckgroundTexture; // To use as buffer. [Todo]=[To_test] -> SetSmooth

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
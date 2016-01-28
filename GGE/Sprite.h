#ifndef __GGE_SPRITES__
# define __GGE_SPRITES__

# include <GCL_CPP/Preprocessor.h>
# include <SFML/Graphics.hpp>
# include <vector>
# include <iostream>
# include <cassert>

namespace GGE
{
	namespace SPRITE
	{
		struct Sheet
		{
			Sheet() = delete;
			Sheet(const Sheet &) = delete;
			Sheet(const Sheet && sheet) = delete;

			Sheet &	operator=(Sheet & sheet) = delete;
			Sheet &	operator=(Sheet && sheet) = delete;
			//{
			//	if (&sheet == this)
			//		throw GCL::Exception("[Error] : Attempting to move a GGE::Sprite::Sheet to itself");
			//	if (sheet._isValid == false)
			//		throw std::logic_error("[Error] : Attempting to move a non-valid GGE::Sprite::Sheet");
			//}

			explicit Sheet(const std::string & texture_path
				, const std::pair<int, int> && dimension
				, const std::pair<int, int> && qty)
				: _texture_path(texture_path)
				, _dimension(dimension)
				, _qty(qty)
			{
				if (!(this->_isValid = this->_texture.loadFromFile(texture_path)))
					std::cerr << "[Error] : Fail to load sprite sheet : [" << texture_path << ']' << std::endl;
				else
				{
					Sheet::LoadSprites(this->_texture, this->_sprites, this->_dimension, this->_qty);
					this->_currentSpriteIterator = this->_sprites.begin();
				}
			}

			DEBUG_INSTRUCTION(const std::vector<sf::Sprite>::const_iterator	Get(void)
			{
				assert(this->_isValid);

				if (this->_currentSpriteIterator == this->_sprites.end())
					this->_currentSpriteIterator = this->_sprites.begin();

				return this->_currentSpriteIterator++;
			})

			static void	LoadSprites(const sf::Texture & texture
				, std::vector<sf::Sprite> & sprites
				, const std::pair<int, int> & dim
				, const std::pair<int, int> & qty)
			{
				const size_t SpriteNbr = qty.first * qty.second;
				int x = 0, y = 0;
				for (size_t it = 0; it < SpriteNbr;)
				{
					sprites.emplace_back(std::move(sf::Sprite()));
					sprites.back().setTexture(texture);
					sprites.back().setTextureRect(sf::IntRect(x * (dim.first / qty.first), y * (dim.second / qty.second), dim.first / qty.first, dim.second / qty.second));
					++it; ++x;
					if (it % qty.first == 0) { x = 0; ++y; }
				}
			}

			const std::vector<sf::Sprite> &	GetContent(void) const
			{
				return this->_sprites;
			}

		protected:
			std::vector<sf::Sprite>					_sprites;
			std::vector<sf::Sprite>::iterator		_currentSpriteIterator;
			const std::string						_texture_path;
			const std::pair<int, int>				_dimension;
			const std::pair<int, int>				_qty;
			bool									_isValid;
			sf::Texture								_texture;
		};
		// A serie of (copy of) sprites, ready for animation
		struct Serie
		{
			Serie() = delete;
			Serie(const Serie  & serie)
				: _sprites(serie._sprites)
				, _currentSpriteIterator(_sprites.begin())
			{}
			Serie(const Serie  && serie)
				: _sprites(std::move(serie._sprites))
				, _currentSpriteIterator(_sprites.begin())
			{}

			explicit Serie(const Sheet & spriteSheet, const size_t qty, const size_t offset)
			{
				this->Load(spriteSheet, qty, offset);
			}
			explicit Serie(const Sheet & spriteSheet)
			{
				this->Load(spriteSheet, spriteSheet.GetContent().size(), 0);
			}

			Serie &										operator+=(const sf::Sprite & sprite)
			{
				this->_sprites.push_back(sprite);
				return *this;
			}
			Serie &										operator++(void)
			{
				if (this->_currentSpriteIterator == this->_sprites.end())
					this->_currentSpriteIterator = this->_sprites.begin();
				this->_currentSpriteIterator++;
				return *this;
			}
			std::vector<sf::Sprite>::iterator &			GetCurrent(void)
			{
				assert(_sprites.size() != 0);

				if (this->_currentSpriteIterator == this->_sprites.end())
					this->_currentSpriteIterator = this->_sprites.begin();

				return this->_currentSpriteIterator;
			}
			std::vector<sf::Sprite>::iterator			Get(void)
			{
				assert(_sprites.size() != 0);

				if (this->_currentSpriteIterator == this->_sprites.end())
					this->_currentSpriteIterator = this->_sprites.begin();

				return this->_currentSpriteIterator++;
			}
			void										Reset(void)
			{
				assert(_sprites.size() != 0);
				this->_currentSpriteIterator = _sprites.begin();
			}

		protected:
			void										Load(const Sheet & spriteSheet, const size_t qty, const size_t offset)
			{
				assert(offset + qty <= spriteSheet.GetContent().size());
				for (size_t it = offset; it < qty; ++it)
					this->_sprites.push_back(spriteSheet.GetContent().at(it));
				this->_currentSpriteIterator = this->_sprites.begin();
			}

			std::vector<sf::Sprite>						_sprites;
			std::vector<sf::Sprite>::iterator			_currentSpriteIterator;
		};
	}
 }

#endif // __GGE_SPRITES__
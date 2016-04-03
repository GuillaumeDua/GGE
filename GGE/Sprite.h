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
		// A whole sprite sheet
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
				, std::pair<int, int> && dimension
				, std::pair<int, int> && qty)
				: _texture_path(texture_path)
				, _dimension(std::move(dimension))
				, _qty(std::move(qty))
				, _spriteDimension{ _dimension.first / _qty.first, _dimension.second / _qty.second }
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

			inline const std::vector<sf::Sprite> &	GetContent(void) const
			{
				return this->_sprites;
			}
			inline const std::pair<int, int> &		GetSpriteDimension(void) const
			{
				return _spriteDimension;
			}
			inline const std::pair<int, int> &		GetDimension(void) const
			{
				return _dimension;
			}
			inline const std::pair<int, int> &		GetQty(void) const
			{
				return _qty;
			}

		protected:
			std::vector<sf::Sprite>					_sprites;
			std::vector<sf::Sprite>::iterator		_currentSpriteIterator;
			const std::string						_texture_path;
			const std::pair<int, int>				_dimension;
			const std::pair<int, int>				_qty;
			const std::pair<int, int>				_spriteDimension;
			bool									_isValid;
			sf::Texture								_texture;
		};
		// A serie of (copy of) sprites, ready for animation
		struct Serie
		{
			using T_SpriteCache = std::vector < sf::Sprite > ;

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

			Serie &											operator+=(const sf::Sprite & sprite)
			{
				this->_sprites.push_back(sprite);
				return *this;
			}
			Serie &											operator++(void)
			{
				if (this->_currentSpriteIterator == this->_sprites.end())
					this->_currentSpriteIterator = this->_sprites.begin();
				this->_currentSpriteIterator++;
				return *this;
			}

			inline const std::vector<sf::Sprite> &			GetSpritesCache(void) const
			{
				return _sprites;
			}
			const std::vector<sf::Sprite>::const_iterator &	GetCurrent(void) const
			{
				assert(_sprites.size() != 0);
				return this->_currentSpriteIterator;
			}
			std::vector<sf::Sprite>::iterator &				GetCurrent(void)
			{
				assert(_sprites.size() != 0);

				if (this->_currentSpriteIterator == this->_sprites.end())
					this->_currentSpriteIterator = this->_sprites.begin();

				return this->_currentSpriteIterator;
			}
			std::vector<sf::Sprite>::iterator				Get(void)
			{
				assert(_sprites.size() != 0);

				if (this->_currentSpriteIterator == this->_sprites.end())
					this->_currentSpriteIterator = this->_sprites.begin();

				return this->_currentSpriteIterator++;
			}
			inline void										Reset(void)
			{
				assert(_sprites.size() != 0);
				this->_currentSpriteIterator = _sprites.begin();
			}
			inline bool										IsOver(void) const
			{
				return _currentSpriteIterator == this->_sprites.cend();
			}

		protected:
			void											Load(const Sheet & spriteSheet, const size_t qty, const size_t offset)
			{
				assert(offset + qty <= spriteSheet.GetContent().size());
				for (size_t it = offset; it < (qty + offset); ++it)
				{
					this->_sprites.push_back(spriteSheet.GetContent().at(it));
					this->_sprites.back().setOrigin(
						static_cast<float>(spriteSheet.GetSpriteDimension().first) / 2.f
						, static_cast<float>(spriteSheet.GetSpriteDimension().second) / 2.f
					);
				}
				this->_currentSpriteIterator = this->_sprites.begin();
			}

			T_SpriteCache									_sprites;
			T_SpriteCache::iterator							_currentSpriteIterator;
		};
	}
 }

#endif // __GGE_SPRITES__
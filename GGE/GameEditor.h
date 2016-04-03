#ifndef __GGE_EDITOR__
# define __GGE_EDITOR__

#include "./__Game.h"
#include <array>
#include <cstddef>

namespace GGE
{
	// @todo : Absract for Button + TextInputBoxm such as "GuiElement"
	namespace Editor
	{
		static GGE::Game gameInstance;

		struct Button : public IEntity, public HitBox
		{
			struct Event
			{
				static const std::string Clicked;
				static const std::string MouseOver;
				static const std::string MouseOut;
			};

			struct Theme
			{
				const std::string DefaultFontPath = "Fonts/Digital System.ttf";

				explicit Theme()
				{
					if (!_font.loadFromFile(DefaultFontPath))
						throw std::runtime_error("Cannot load default font : [" + DefaultFontPath + "]");
				}
				Theme(const Theme&) = default;
				Theme(Theme &&) = delete;

				sf::Font	_font;
				sf::Color	_backgroundColor = { 100, 150, 200 };
				sf::Color	_textColor = sf::Color::White;
				sf::Color	_borderColor = sf::Color::Black;
				float		_borderThickness = 3;

				const Theme & operator >> (sf::Shape & shape) const
				{
					shape.setFillColor(_backgroundColor);
					shape.setOutlineColor(_borderColor);
					shape.setOutlineThickness(3);
					return *this;
				}
				const Theme & operator >> (sf::Text & text) const
				{
					text.setFont(_font);
					text.setColor(_textColor);
					return *this;
				}
				const Theme & operator >> (Button & button) const
				{
					*this >> button._shape;
					*this >> button._textGraphicalElem;
					return *this;
				}

			}	static DefaultTheme;

			Theme _theme = DefaultTheme;

			explicit Button(const HitBox & hb, const std::string & text, std::function<void(void)> onClickCB = {})
				: HitBox(hb)
				, _shape(sf::Vector2f(static_cast<float>(hb.GetSize().first), static_cast<float>(hb.GetSize().second)))
			{
				*this << _theme;

				_textGraphicalElem.setString(text);

				if (onClickCB)
					this->on(Button::Event::Clicked) += std::move(onClickCB);

				this->on(Button::Event::MouseOver) += std::move([this]() mutable
				{
					_theme._backgroundColor.r += 20;
					_theme._backgroundColor.g += 20;
					_theme._backgroundColor.b += 20;
					_shape.setFillColor(_theme._backgroundColor);
				});
				this->on(Button::Event::MouseOut) += std::move([this]() mutable
				{
					_theme._backgroundColor.r -= 20;
					_theme._backgroundColor.g -= 20;
					_theme._backgroundColor.b -= 20;
					_shape.setFillColor(_theme._backgroundColor);
				});

				gameInstance.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseButtonPressed, [&, this](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
				{
					HitBox cursorHB{ { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) }, { 1, 1 } };

					if (GGE::CollisionEngine::Algorythms::AABB::IsCollision(cursorHB, *this))
					{
						this->Notify(Button::Event::Clicked);
					}

					return GGE::UserEventsHandler::RegisteredCBReturn::OK;
				}));
				gameInstance.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseMoved, [&, this](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
				{
					HitBox cursorHB{ { static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y) }, { 1, 1 } };

					if (GGE::CollisionEngine::Algorythms::AABB::IsCollision(cursorHB, *this))
					{
						if (!this->IsMouseOver())
						{
							this->_isMouseOver = true;
							this->Notify(Button::Event::MouseOver);
						}
					}
					else if (this->IsMouseOver())
					{
						this->_isMouseOver = false;
						this->Notify(Button::Event::MouseOut);
					}

					return GGE::UserEventsHandler::RegisteredCBReturn::OK;
				}));
			}

			inline Button &						operator<<(const Theme & theme)
			{
				theme >> *this;
				return *this;
			}

			void								Draw(sf::RenderWindow & renderWindow)
			{

				_textGraphicalElem.setPosition(_position.first + 10.f, _position.second);
				_shape.setPosition(_position.first, _position.second);

				renderWindow.draw(_shape);
				renderWindow.draw(_textGraphicalElem);
			}
			bool								Behave(void)
			{
				this->ResolvePendingNotifications();
				return true;
			}

			inline const bool					IsMouseOver(void) const
			{
				return _isMouseOver;
			}
			inline const std::string			GetText(void) const
			{
				return _textGraphicalElem.getString();
			}
			inline void							SetText(const std::string & text)
			{
				_textGraphicalElem.setString(text);
			}

		protected:

			bool								_isMouseOver = false;
			sf::RectangleShape					_shape;
			sf::Text							_textGraphicalElem;
		};

		Button::Theme Button::DefaultTheme;

		const std::string Button::Event::Clicked = "onClicked";
		const std::string Button::Event::MouseOver = "onMouseOver";
		const std::string Button::Event::MouseOut = "onMouseOut";

		struct TextInputBox : public Button
		{
			struct Event
			{
				static const std::string TextChanged;
			};

			explicit TextInputBox(const HitBox & hb, const std::string & text)
				: Button(hb, text)
			{
				gameInstance.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseButtonPressed, [&, this](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
				{
					HitBox cursorHB{ { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) }, { 1, 1 } };

					_isFocusAquiered = GGE::CollisionEngine::Algorythms::AABB::IsCollision(cursorHB, *this);

					return GGE::UserEventsHandler::RegisteredCBReturn::OK;
				}));
				gameInstance.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::TextEntered, [&, this](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
				{
					if (this->IsFocusAquiered())
					{
						if (event.text.unicode == 8)
						{
							if (this->_textGraphicalElem.getString().getSize() == 0)
								return GGE::UserEventsHandler::RegisteredCBReturn::OK;
							SetText(this->_textGraphicalElem.getString().substring(0, this->_textGraphicalElem.getString().getSize() - 1));
						}
						else
							SetText(this->_textGraphicalElem.getString() + static_cast<char>(event.text.unicode));

						this->Notify(Event::TextChanged);
					}

					return GGE::UserEventsHandler::RegisteredCBReturn::OK;
				}));
			}

			inline const bool	IsFocusAquiered(void) const
			{
				return _isFocusAquiered;
			}

		protected:
			bool _isFocusAquiered = false;
		};

		const std::string TextInputBox::Event::TextChanged = "onTextChanged";


		struct DynamicAnimationsStack
			: public IEntity	// Draw
			, public HitBox
		{
			explicit DynamicAnimationsStack(const HitBox::PositionType && pos, const HitBox::SizeType && dimMax)
				: HitBox(pos, dimMax)
				, _drawAdapter(pos, dimMax, { 0, 0 })
			{}

			std::shared_ptr<GGE::SPRITE::Sheet> _spriteSheet = nullptr;
			std::vector<GGE::SPRITE::Serie>	_spritesSeries;

			void	Reload(void)
			{
				if (_spriteSheet == nullptr)
					throw std::runtime_error("DynamicAnimationsStack::ReloadAnimations : NullPtr");

				_spritesSeries.clear();

				for (size_t it = 0; it < _spriteSheet->GetQty().second; ++it)
				{
					GGE::SPRITE::Serie serie{ *_spriteSheet, static_cast<size_t>(_spriteSheet->GetQty().first), it * (_spriteSheet->GetQty().first) };
					_spritesSeries.emplace_back(serie);
				}
				_drawAdapter._spriteSize = _spriteSheet->GetSpriteDimension();
				// [todo] : Remove blank tiles
			}
			void	Draw(sf::RenderWindow & renderWindow) override
			{
				if (_spriteSheet == nullptr)
					return;

				_drawAdapter.ResetIt();

				for (auto & spriteSerie : _spritesSeries)
				{
					auto & sprite = *(spriteSerie.GetCurrent());
					_drawAdapter.Draw(sprite, renderWindow);
				}
			}
			bool	Behave(void) override
			{
				for (auto & spriteSerie : _spritesSeries)
					++spriteSerie;
				// check reloadRequested event
				return true;
			}
		protected:
			struct SpriteSheetDrawingAdapter
			{
				explicit SpriteSheetDrawingAdapter(const HitBox::PositionType & pos, const HitBox::SizeType & size, const HitBox::SizeType & spriteSize)
					: _position(pos)
					, _size(size)
					, _spriteSize(spriteSize)
				{}

				void					Draw(sf::Sprite & sprite, sf::RenderWindow & renderWindow)
				{
					sprite.setPosition(_position.first + _spriteSize.first * _drawnIt++, _position.second);
					renderWindow.draw(sprite);
				}
				inline void				ResetIt(void)
				{
					_drawnIt = 0;
				}

				unsigned int			_drawnIt = 0;
				HitBox::PositionType	_position;
				HitBox::SizeType		_size;
				HitBox::SizeType		_spriteSize;
			}	_drawAdapter;
		};

		void	AnimationEditionTool(void)
		{
			std::shared_ptr<TextInputBox> txtBx_spriteSheetPath = std::make_shared<TextInputBox>(
				HitBox{ { 50.f, 50.f }, { 600, 50 } }
				, std::string("C:/DEV/PROJECTS/GGE/GGE/SPRITES/GusGameOfLife.bmp")
				);

			GCL::Vector<std::shared_ptr<TextInputBox>> array_txtBx_DimQty =
			{
				std::make_shared<TextInputBox>(
					HitBox{ { 50.f, 100.f }, { 50, 50 } }
					, std::string("500")
				),
				std::make_shared<TextInputBox>(
					HitBox{ { 100.f, 100.f }, { 50, 50 } }
					, std::string("500")
				),
				std::make_shared<TextInputBox>(
					HitBox{ { 150.f, 100.f }, { 50, 50 } }
					, std::string("10")
				),
				std::make_shared<TextInputBox>(
					HitBox{ { 200.f, 100.f }, { 50, 50 } }
					, std::string("10")
				)
			};

			std::shared_ptr<DynamicAnimationsStack> das = std::make_shared<DynamicAnimationsStack>(DynamicAnimationsStack{ { 50.f, 200.f }, { 500, 500 } });

			std::shared_ptr<Button> btn_load = std::make_shared<Button>(
				HitBox{ { 660.f, 50.f }, { 100, 50 } }
				, "Load"
				, [&txtBx_spriteSheetPath, &array_txtBx_DimQty, &das]()
			{ 
				std::cout << "Loading : [" << txtBx_spriteSheetPath->GetText() << "]" << std::endl; 

				auto dim = std::make_pair(std::stoi(array_txtBx_DimQty.at(0)->GetText()), std::stoi(array_txtBx_DimQty.at(1)->GetText()));
				auto qty = std::make_pair(std::stoi(array_txtBx_DimQty.at(2)->GetText()), std::stoi(array_txtBx_DimQty.at(3)->GetText()));

				auto ptr = new GGE::SPRITE::Sheet(
					txtBx_spriteSheetPath->GetText()
					, std::move(dim)
					, std::move(qty)
					);
				das->_spriteSheet.reset(ptr);
				das->Reload();
			});


			auto scene = std::make_shared<GGE::Game::SceneType>(
				"SPRITES/bg_blue.png",
				std::initializer_list < std::shared_ptr<IEntity> >
			{
				txtBx_spriteSheetPath
					, btn_load
					, std::static_pointer_cast<IEntity>(das)
			});
			for (auto & elem : array_txtBx_DimQty)
				scene->operator+=(std::static_pointer_cast<IEntity>(elem));
			gameInstance += scene;

			gameInstance.Start();
		}

		void	Test(void)
		{
			try
			{
				AnimationEditionTool();
			}
			catch (const std::exception & ex)
			{
				std::cerr << "[ERROR] : STD Exception catch : [" << ex.what() << "]" << std::endl;
			}
			catch (...)
			{
				std::cerr << "[FATAL_ERROR] : Unknown element catch" << std::endl;
			}
		}

	}
}

#endif // __GGE_EDITOR__
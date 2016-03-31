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
				sf::Color	_backgroundColor = {100, 150, 200};
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

				std::cout << "Button addr : 0x" << this << std::endl;

				if (onClickCB)
					this->on(Button::Event::Clicked) += std::move(onClickCB);

				this->on(Button::Event::MouseOver) += std::move([this]() mutable
				{
					_theme._backgroundColor.r += 20;
					_theme._backgroundColor.g += 20;
					_theme._backgroundColor.b += 20;
				});
				this->on(Button::Event::MouseOut) += std::move([this]() mutable
				{
					_theme._backgroundColor.r -= 20;
					_theme._backgroundColor.g -= 20;
					_theme._backgroundColor.b -= 20;
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
					HitBox cursorHB{ { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) }, { 1, 1 } };

					if (GGE::CollisionEngine::Algorythms::AABB::IsCollision(cursorHB, *this))
					{
						if (!this->IsMouseOver())
							this->Notify(Button::Event::MouseOver);
					}
					else if (this->IsMouseOver())
						this->Notify(Button::Event::MouseOut);

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
				_textGraphicalElem.setPosition(_position.first, _position.second);
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
	
		void	Test(void)
		{
			std::shared_ptr<Button> button1 = std::make_shared<Button>(
				HitBox{ { 50.f, 100.f }, { 200, 100 } }
				, "button 1"
				, [](){ std::cout << "Button 1 clicked" << std::endl; }
			);
			std::shared_ptr<Button> button2 = std::make_shared<Button>(
				HitBox{ { 350.f, 100.f }, { 200, 100 } }
				, "button 2"
				, [](){ std::cout << "Button 2 clicked" << std::endl; }
			);

			std::shared_ptr<TextInputBox> textBox1 = std::make_shared<TextInputBox>(
				HitBox{ { 200.f, 300.f }, { 200, 100 } }
				, std::string("TextBox 1")
			);

			std::cout << "Button  1 addr : 0x" << button1.get() << std::endl;
			std::cout << "Button  2 addr : 0x" << button2.get() << std::endl;
			std::cout << "TextBox 1 addr : 0x" << textBox1.get() << std::endl;

			textBox1->on(TextInputBox::Event::TextChanged) += [&textBox1](){ std::cout << "TextBox 1 : TextChanged to : [" << textBox1->GetText() << ']' << std::endl; };

			gameInstance += std::make_shared<GGE::Game::SceneType>(
				"SPRITES/bg_blue.png",
				std::initializer_list < std::shared_ptr<IEntity> >
			{
				button1
				, button2
				, textBox1
			});

			gameInstance.Start();
		}

	}
}

#endif // __GGE_EDITOR__
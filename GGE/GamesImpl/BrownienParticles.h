// Cannot be multi-included

#include "../GameEngine.h"
#include "../Entity.h"

namespace GameImpl
{
	namespace BrownienParticles
	{
		static const GGE::GameEngine::Configuration GE_Configuration =
		{
			{
				60.f
				,{
					[]() { std::cerr << "[Warning] : Frame drop detected !" << std::endl; }
					// , ...
				}
			}
			,{ 800, 600, 32, "[Brownien particles] : Instance rendering" }
		};

		struct ParticleTrait
		{
			friend GGE::Entity < ParticleTrait >;

			enum Status
			{
				Moving
				, Default = Moving
			};

			using Behavior = std::map < Status, std::function<bool(GGE::Entity<ParticleTrait>&)> >;
			using Animation = std::map < Status, GGE::SPRITE::Serie >;

			static const GGE::SPRITE::Sheet			gSpriteSheet_walking;
			static const GGE::SPRITE::Sheet			gSpriteSheet_Destroying;

			static const Behavior					_behavior;
			static const Animation					_animation;

			static const std::pair<int, int>		_size;
		};

		using Particle = GGE::Entity < ParticleTrait >;

		static void	Run()	// throw
		{
			try
			{
				GGE::GameEngine game(GE_Configuration);

				game += std::make_shared<GGE::GameEngine::SceneType>(
					"SPRITES/bg_blue.png",
					std::initializer_list < std::shared_ptr<IEntity> >
				{
					std::make_shared<Particle>(std::move(std::make_pair(300.f, 300.f)))
				});

				game.Start();
			}
			catch (const std::exception & ex)
			{
				std::cerr
					<< "[GameImpl::BrownienParticles]::[FATAL_ERROR] : std::exception catch" << std::endl
					<< ex.what() << std::endl
					;
			}
			catch (...)
			{
				std::cerr
					<< "[GameImpl::BrownienParticles]::[FATAL_ERROR] : Unknow element catch" << std::endl
					;
			}
		}
	}
}
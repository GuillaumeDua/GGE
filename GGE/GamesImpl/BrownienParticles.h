// Cannot be multi-included

#include "../GameEngine.h"
#include "../Entity.h"

#include <random>

namespace GameImpl
{
	namespace BrownienParticles
	{
		static const GGE::GameEngine::Configuration GE_Configuration =
		{
			{
				20.f
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

		struct PoissonRandomGenerator
		{
			const float _mean = 100.f;

			std::default_random_engine generator;
			std::poisson_distribution<int> distribution = std::poisson_distribution<int>{ _mean };

			inline int Get(void)
			{
				return distribution(generator);
			}
		} static randomGen;

		static void	Run(void)
		{
			try
			{
				GGE::GameEngine game(GE_Configuration);

				std::vector<std::shared_ptr<GGE::IEntity>> particlesVector(100);
				std::generate(
					particlesVector.begin()
					, particlesVector.end()
					, [&]() -> std::shared_ptr<GGE::IEntity>
					{
						return std::make_shared<Particle>(std::move(
							std::make_pair(
								static_cast<float>(randomGen.Get() % GE_Configuration._screenDim._x)
							,	static_cast<float>(randomGen.Get() % GE_Configuration._screenDim._y))));
					}
				);

				std::shared_ptr<GGE::GameEngine::SceneType> scene = std::make_shared<GGE::GameEngine::SceneType>
				(
					"SPRITES/bg_blue.png",
					std::initializer_list < std::shared_ptr<GGE::IEntity> >{}
				);
				scene->GetContent().insert(scene->GetContent().end(), particlesVector.begin(), particlesVector.end());

				game += scene;

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
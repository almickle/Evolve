#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "Actor.h"
#include "Camera.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "Light.h"
#include "Scene.h"
#include "SceneData.h"
#include "SystemManager.h"
#include "Types.h"

void Scene::Load( SystemManager* systemManager, JsonSerializer& serializer )
{
	auto* resourceManager = systemManager->GetResourceManager();

	Deserialize( serializer );

	// Initialize scene constant buffer
	CameraData camData{
		.view{activeCamera->GetViewMatrix()},
		.proj{activeCamera->GetProjectionMatrix()},
		.position{activeCamera->GetPosition()}
	};
	LightData lightData[8]{};
	lights.resize( lights.size() );
	for( size_t i = 0; i < lights.size() && i < 8; ++i ) {
		lightData[i].type = lights[i]->GetType();
		lightData[i].position = lights[i]->GetPosition();
		lightData[i].intensity = lights[i]->GetIntensity();
		lightData[i].color = lights[i]->GetColor();
	}
	SceneData sceneData{
		.camera = camData,
		.lights = {lightData[0], lightData[1], lightData[2], lightData[3],
				   lightData[4], lightData[5], lightData[6], lightData[7]},
		.numLights = static_cast<uint>( lights.size() )
	};
	sceneConstantBuffer = resourceManager->CreateConstantBuffer( &sceneData, sizeof( SceneData ), "SceneData" );
	auto staticData = staticInstanceManager->GetAllInstanceData();
	staticData.resize( 1024 );
	staticStructuredBuffer = resourceManager->CreateStaticStructuredBuffer<ActorTransformData>( staticData, "StaticActors" );
	auto dynamicData = dynamicInstanceManager->GetAllInstanceData();
	dynamicData.resize( 1024 );
	dynamicStructuredBuffer = resourceManager->CreateStaticStructuredBuffer<ActorTransformData>( dynamicData, "StaticActors" );
}

std::string Scene::Serialize( JsonSerializer& serializer ) const
{
	return "";
}

void Scene::Deserialize( JsonSerializer& serializer )
{
	try
	{
		DeserializeBaseAsset( serializer );

		// Lights
		lights.clear();
		auto lightArray = serializer.GetSubObject( "lights" );
		for( const auto& l : lightArray ) {
			auto light = std::make_unique<Light>();
			if( l.contains( "type" ) ) light->SetType( l.at( "type" ).get<LightType>() );
			if( l.contains( "position" ) ) {
				auto& pos = l.at( "position" );
				light->SetPosition( { pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>() } );
			}
			if( l.contains( "intensity" ) ) light->SetIntensity( l.at( "intensity" ).get<float>() );
			if( l.contains( "color" ) ) {
				auto& col = l.at( "color" );
				light->SetColor( { col[0].get<float>(), col[1].get<float>(), col[2].get<float>() } );
			}
			lights.push_back( std::move( light ) );
		}

		// Cameras
		cameras.clear();
		auto cameraArray = serializer.GetSubObject( "cameras" );
		for( const auto& c : cameraArray ) {
			auto camera = std::make_unique<Camera>();
			if( c.contains( "position" ) ) {
				auto& pos = c.at( "position" );
				camera->SetPosition( { pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>() } );
			}
			cameras.push_back( std::move( camera ) );
		}

		// Active camera
		uint idx = serializer.Read<uint>( "activeCamera" );
		if( idx < cameras.size() ) {
			activeCamera = cameras[idx].get();
		}
		else {
			activeCamera = nullptr;
		}

		// Static Actors
		staticActors.clear();
		auto staticArray = serializer.GetSubObject( "staticActors" );
		for( const auto& a : staticArray ) {
			auto actor = std::make_unique<Actor>();
			if( a.contains( "model" ) ) actor->SetModel( a.at( "model" ).get<std::string>() );
			if( a.contains( "position" ) ) {
				auto& pos = a.at( "position" );
				actor->SetPosition( { pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>() } );
			}
			if( a.contains( "rotation" ) ) {
				auto& rot = a.at( "rotation" );
				actor->SetRotation( { rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>(), rot[3].get<float>() } );
			}
			if( a.contains( "scale" ) ) {
				auto& scale = a.at( "scale" );
				actor->SetScale( { scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>() } );
			}
			if( a.contains( "velocity" ) ) {
				auto& vel = a.at( "velocity" );
				actor->SetVelocity( { vel[0].get<float>(), vel[1].get<float>(), vel[2].get<float>() } );
			}
			if( a.contains( "acceleration" ) ) {
				auto& acc = a.at( "acceleration" );
				actor->SetAcceleration( { acc[0].get<float>(), acc[1].get<float>(), acc[2].get<float>() } );
			}
			staticInstanceManager->AddInstance( actor.get() );
			staticActors.push_back( std::move( actor ) );
		}

		for( auto& actor : staticActors ) {
		}

		// Dynamic Actors
		dynamicActors.clear();
		auto dynamicArray = serializer.GetSubObject( "dynamicActors" );
		for( const auto& a : dynamicArray ) {
			auto actor = std::make_unique<Actor>();
			if( a.contains( "model" ) ) actor->SetModel( a.at( "model" ).get<std::string>() );
			if( a.contains( "position" ) ) {
				auto& pos = a.at( "position" );
				actor->SetPosition( { pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>() } );
			}
			if( a.contains( "rotation" ) ) {
				auto& rot = a.at( "rotation" );
				actor->SetRotation( { rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>(), rot[3].get<float>() } );
			}
			if( a.contains( "scale" ) ) {
				auto& scale = a.at( "scale" );
				actor->SetScale( { scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>() } );
			}
			if( a.contains( "velocity" ) ) {
				auto& vel = a.at( "velocity" );
				actor->SetVelocity( { vel[0].get<float>(), vel[1].get<float>(), vel[2].get<float>() } );
			}
			if( a.contains( "acceleration" ) ) {
				auto& acc = a.at( "acceleration" );
				actor->SetAcceleration( { acc[0].get<float>(), acc[1].get<float>(), acc[2].get<float>() } );
			}
			dynamicInstanceManager->AddInstance( actor.get() );
			dynamicActors.push_back( std::move( actor ) );
		}
	}
	catch( const std::exception& )
	{
		throw std::runtime_error( "Failed to deserialize Scene asset" );
	}
}
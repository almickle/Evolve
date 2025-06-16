#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "MaterialNode.h"
#include "NodeTypes.h"

class MaterialOutputNode : public MaterialNode {
public:
	MaterialOutputNode( const std::string& name = "MaterialOutputNode" )
		: MaterialNode( 7, 1, 0, name )
	{
		AddInput( 0, NodeSlot{ "baseColor", DirectX::XMFLOAT4{1.0f, 1.0f, 1.0f, 1.0f} } );
		AddInput( 1, NodeSlot{ "metallic", 0.0f } );
		AddInput( 2, NodeSlot{ "roughness", 1.0f } );
		AddInput( 3, NodeSlot{ "normal",  DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
		AddInput( 4, NodeSlot{ "emissive", DirectX::XMFLOAT4{0.0f, 0.0f, 0.0f, 0.0f} } );
		AddInput( 5, NodeSlot{ "opacity", 1.0f } );
		AddInput( 6, NodeSlot{ "ambientOcculusion", 1.0f } );
		AddOutput( 0, NodeSlot{ "color", DirectX::XMFLOAT4{1.0f, 1.0f, 1.0f, 1.0f} } );
	}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = R"(    
            // Camera and position
            float3 viewDir = normalize(cameraPosition - vertexData.worldPos);

            // PBR constants
            float3 F0 = lerp(float3(0.04, 0.04, 0.04), input.baseColor.rgb, input.metallic);

            float3 Lo = float3(0, 0, 0);

            // Loop over lights
            [unroll(numLights)]
            for (uint i = 0; i < numLights; ++i)
            {
                LightData light = lights[i];
                float3 L = normalize(light.position - vertexData.worldPos);
                float3 H = normalize(viewDir + L);

                // Attenuation (simple, can be improved)
                float distance = length(light.position - vertexData.worldPos);
                float attenuation = 1.0 / (distance * distance);

                // Diffuse
                float NdotL = max(dot(input.normal.xyz, L), 0.0);

                // Cook-Torrance BRDF
                float NDF = DistributionGGX(input.normal.xyz, H, input.roughness);
                float G = GeometrySmith(input.normal.xyz, viewDir, L, input.roughness.r);
                float3 F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

                float3 numerator = NDF * G * F;
                float denominator = 4 * max(dot(input.normal.xyz, viewDir), 0.0) * NdotL + 0.001;
                float3 specular = numerator / denominator;

                float3 kS = F;
                float3 kD = 1.0 - kS;
                kD *= 1.0 - input.metallic;

                float3 irradiance = light.color * light.intensity * attenuation;

                Lo += (kD * input.baseColor.rgb / PI + specular) * irradiance * NdotL;
            }

            // Ambient
            float3 ambient = input.ambientOcculusion * input.baseColor.rgb * 0.03;

            // Final color
            float3 color = ambient + Lo + input.emissive.rgb;
            color = color / (color + 1.0); // HDR tonemapping
            color = pow(color, 1.0 / 2.2); // Gamma correction

            output.color = float4(color, input.opacity);
)";

		std::string shaderFunction = std::format( "{}\n{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
};
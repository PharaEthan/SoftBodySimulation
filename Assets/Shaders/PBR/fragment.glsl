precision highp float;

out vec4 frag_color;

in vec3 vColor;
in vec3 vPosition;
in vec3 vPositionW;
in vec3 vNormal;
in vec3 vNormalW;
in vec2 vUV;
in mat4 vNormalMatrix;

#ifdef SHADOW_MAP
in vec4 vPositionShadow;
uniform vec3 lightDirection;
vec2 poissonDisk[8] = vec2[](
vec2(-0.94201624, -0.39906216),
vec2(0.94558609, -0.76890725),
vec2(-0.094184101, -0.92938870),
vec2(0.34495938, 0.29387760),
vec2(-0.91588581, 0.45771432),
vec2(-0.81544232, -0.87912464),
vec2(-0.38277543, 0.27676845),
vec2(0.97484398, 0.75648379)
);
#endif

uniform mat4 world;
uniform mat4 normalMatrix;

uniform int pointLightCount;
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int directionalLightCount;
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];

uniform vec3 cameraPosition;

uniform bool lightingEnabled;

uniform vec3 albedoColor;
uniform vec3 ambientColor;
uniform vec3 alphaColor;

uniform float metallic;
uniform float roughness;
uniform float ao;

layout(binding = 0) uniform sampler2D albedoTexture;
layout(binding = 1) uniform sampler2D roughnessTexture;
layout(binding = 2) uniform sampler2D normalTexture;
layout(binding = 3) uniform sampler2D shadowMap;

#ifdef SHADOW_MAP
float random(vec4 seed4) {
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float getShadowFactor(vec4 positionShadow, vec3 normal) {
    vec3 shadowCoord = positionShadow.xyz / positionShadow.w;
    shadowCoord = shadowCoord * 0.5 + 0.5;

    float epsilon = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005) * 0.002;

    float currentDepth = shadowCoord.z;

    float shadowMultiplier = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int i = 0; i < 8; i++) {
                vec2 offset = poissonDisk[i] / 1000.0;
                vec2 texelCoord = shadowCoord.xy + vec2(x, y) * texelSize + offset;
                float texelDepth = texture(shadowMap, texelCoord).r;

                shadowMultiplier += currentDepth - epsilon > texelDepth ? 0.2 : 1.0;
            }
        }
    }
    shadowMultiplier /= 9.0 * 8.0;


    if (shadowCoord.z > 1.0) {
        shadowMultiplier = 1.0;
    }

    return shadowMultiplier;
}
#endif

#define PI 3.14159265359

// http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // récupère les vecteurs du triangle composant le pixel
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // résout le système linéaire
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construit une trame invariante à l'échelle
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal(vec3 N, vec3 V, vec2 texcoord)
{
    // N, la normale interpolée et
    // V, le vecteur vue (vertex dirigé vers l'œil)
    vec3 map = texture(normalTexture, texcoord ).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


void main() {
    vec3 color = vec3(0.0);

    vec3 normal = vNormal;

    vec3 V = normalize(cameraPosition - vPositionW);

    #ifdef NORMAL_TEXTURE
    normal = perturb_normal(normal, V, vUV);
    #endif

    vec3 N = normalize(vec3(normalMatrix * vec4(normal, 0.0)));


    if (lightingEnabled) {
        vec3 albedo = albedoColor;
        #ifdef ALBEDO_TEXTURE
        albedo *= pow(texture(albedoTexture, vUV).rgb, vec3(2.2));
        #endif

        float roughness = roughness;
        #ifdef ROUGHNESS_TEXTURE
        roughness = texture(roughnessTexture, vUV).r;
        #endif

        #ifdef ALPHA_COLOR
        if (albedo == alphaColor) discard;
        #endif

        vec3 Lo = vec3(0.0);

        vec3 F0 = vec3(0.04);
        F0      = mix(F0, albedo, metallic);

        for (int i = 0; i < pointLightCount; i++) {
            vec3 L = normalize(pointLights[i].position - vPositionW);
            vec3 H = normalize(V + L);

            float distance = length(pointLights[i].position - vPositionW);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = pointLights[i].color * attenuation;

            vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float NDF = DistributionGGX(N, H, roughness);
            float G   = GeometrySmith(N, V, L, roughness);

            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
            vec3 specular     = numerator / denominator;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;

            kD *= 1.0 - metallic;

            float NdotL = max(dot(N, L), 0.01);
            Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        }

        for (int i = 0; i < directionalLightCount; i++) {
            vec3 L = normalize(directionalLights[i].direction);
            vec3 H = normalize(V + L);

            vec3 radiance = directionalLights[i].color;

            vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float NDF = DistributionGGX(N, H, roughness);
            float G   = GeometrySmith(N, V, L, roughness);

            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
            vec3 specular     = numerator / denominator;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;

            kD *= 1.0 - metallic;

            float NdotL = max(dot(N, L), 0.01);
            Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        }

        color += Lo;
    }

    vec3 ambientColor = ambientColor;
    #ifdef AMBIENT_TEXTURE
    ambientColor = texture(ambientTexture, vUV).rgb;
    #endif

    #ifdef ALPHA_COLOR
    if (ambientColor == alphaColor) discard;
    #endif

    color += ambientColor;

    #ifdef SHADOW_MAP
    color *= getShadowFactor(vPositionShadow, N);
    #endif

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    frag_color = vec4(color, 1.0);
}

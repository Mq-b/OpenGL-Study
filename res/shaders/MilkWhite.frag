#version 330 core
in vec3 vNormal;
in vec3 vPos;
out vec4 FragColor;
uniform vec3 viewPos;
void main(){
    vec3 N = normalize(vNormal);
    vec3 V = normalize(viewPos - vPos);
    
    // 1. 模拟环境全局光 (Hemisphere Lighting)
    // 顶部光（淡暖色）与底部光（暗冷色）混合
    vec3 skyColor = vec3(1.0, 1.0, 0.95); 
    vec3 groundColor = vec3(0.2, 0.2, 0.25);
    float hemi = max(0.0, dot(N, vec3(0, 1, 0)) * 0.5 + 0.5);
    vec3 ambient = mix(groundColor, skyColor, hemi) * 0.4;
    
    // 2. 主光源 (定向光)
    vec3 L = normalize(vec3(0.5, 1.0, 0.8));
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * vec3(0.6, 0.6, 0.55);

    // 3. 边缘光 (Rim Lighting) - 增强体积感和全局光质感
    float rim = pow(1.0 - max(dot(N, V), 0.0), 4.0);
    vec3 rimColor = rim * vec3(0.5, 0.5, 0.5);

    // 4. 奶白材质颜色合成
    vec3 baseColor = vec3(0.95, 0.94, 0.88); // 经典奶白色
    vec3 finalColor = (ambient + diffuse + rimColor) * baseColor;

    // Gamma 校正，让颜色更自然，不发灰
    finalColor = pow(finalColor, vec3(1.0/2.2));
    FragColor = vec4(finalColor, 1.0);
}
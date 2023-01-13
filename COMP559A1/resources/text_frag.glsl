#version 330 compatibility
in vec2 TexCoords;
uniform sampler2D text;
in vec4 fragColor;

void main() {
    // for seeing the size of the glyphs
    //vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r * .5 + 0.5);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r );
    gl_FragColor = fragColor * sampled;
}
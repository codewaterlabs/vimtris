let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    uniform mat3 layout;
    varying vec2 vPosition;
    void main() {
        vPosition = position;
        vec2 pos = (vec3(position, 1.0) * layout).xy;
        gl_Position = vec4(pos, 0.0, 1.0);
    }
|};

let fragmentSource = {|
    precision mediump float;
    varying vec2 vPosition;

    uniform sampler2D tiles;

    const float numCols = 12.0;
    const float numRows = 26.0;

    void main() {
        // Perspective coord
        vec2 aspect = vec2(numCols / numRows, 1.0);
        vec2 persp = vPosition + vec2(
            vPosition.x * 0.08,
            0.0
        );
        persp.x = clamp(persp.x, -1.0, 1.0);
        vec2 tilePos = vec2((persp.x + 1.0) * 0.5, (persp.y - 1.0) * -0.5);
        tilePos.y = tilePos.y - 0.03;
        float tile = texture2D(tiles, tilePos).x;
        vec4 tileColor = (tile > 0.0) ?  vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
        gl_FragColor = tileColor;
    }
|};

open Gpu;

let makeNode = tilesTex => {
  /* Use two textures */
  let toTex = Gpu.Texture.makeEmptyRgb();
  let tempTex = Gpu.Texture.makeEmptyRgb();
  /* First draw unblurred */
  let unblurred =
    Scene.makeNode(
      ~key="tileShadows",
      ~vertShader=Shader.make(vertexSource),
      ~fragShader=Shader.make(fragmentSource),
      ~textures=[("tiles", tilesTex)],
      ~drawTo=Scene.TextureItem(toTex),
      ()
    );
  Blur2.makeNode(unblurred, toTex, tempTex, 4.0, 4.0);
};

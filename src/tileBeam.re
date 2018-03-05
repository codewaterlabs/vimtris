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

    void main() {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
|};

open Gpu;

let makeNode = vo =>
  Scene.makeNode(
    ~key="beams",
    ~vertShader=Shader.make(vertexSource),
    ~fragShader=Shader.make(fragmentSource),
    ~drawTo=Scene.TextureRGB,
    ~clearOnDraw=true,
    ~vo,
    ()
  );

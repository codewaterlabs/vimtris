let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    attribute float fromDrop;
    uniform mat3 layout;
    varying float vFromDrop;
    void main() {
        vFromDrop = fromDrop;
        vec2 pos = (vec3(position, 1.0) * layout).xy;
        gl_Position = vec4(pos, 0.0, 1.0);
    }
|};

let fragmentSource = {|
    precision mediump float;
    varying float vFromDrop;
    uniform float sinceDrop;

    void main() {
        float c = max(0.0, vFromDrop - sinceDrop);
        gl_FragColor = vec4(c, c, c, 1.0);
    }
|};

open Gpu;

let makeNode = vo =>
  Scene.makeNode(
    ~key="dropBeams",
    ~vertShader=Shader.make(vertexSource),
    ~fragShader=Shader.make(fragmentSource),
    ~uniforms=[("sinceDrop", Scene.UFloat.zero())],
    ~drawTo=Scene.TextureRGBDim(1024),
    ~clearOnDraw=true,
    ~vo,
    ()
  );

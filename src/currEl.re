let currElVertex = {|
    precision mediump float;
    attribute vec2 position;
    uniform mat3 translation;
    uniform mat3 layout;
    uniform mat3 sdfTilesMat;
    varying vec2 vPosition;
    varying vec2 vTexPos;
    void main() {
        vPosition = (vec3(position, 1.0) * translation).xy;
        vec3 transformed = vec3(vPosition, 1.0) * layout;
        vTexPos = (vec3(vPosition, 1.0) * sdfTilesMat).xy;
        gl_Position = vec4(transformed.xy, 0.0, 1.0);
    }
|};

let currElFragment = {|
    precision mediump float;
    varying vec2 vPosition;
    varying vec2 vTexPos;

    uniform vec3 elColor;
    uniform sampler2D sdfTiles;

    void main() {
        vec3 sdfColor = texture2D(sdfTiles, vTexPos).xyz;
        gl_FragColor = vec4(mix(elColor, sdfColor, 0.4), 1.0);
    }
|};

open Gpu;

let makeNode = (elState: SceneState.elState, sdfTiles) =>
  Scene.makeNode(
    ~key="currEl",
    ~vertShader=Shader.make(currElVertex),
    ~fragShader=Shader.make(currElFragment),
    ~vo=elState.vo,
    ~partialDraw=true,
    ~uniforms=[("elColor", elState.color), ("translation", elState.pos)],
    ~textures=[("sdfTiles", Scene.SceneTex.node(sdfTiles))],
    ()
  );

let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    uniform mat3 layout;
    uniform mat3 sdfTilesMat;
    varying vec2 sdfPos;
    varying vec2 vPosition;
    void main() {
        vPosition = position;
        vec3 transformed = vec3(position, 1.0) * layout;
        sdfPos = (vec3(position, 1.0) * sdfTilesMat).xy;
        gl_Position = vec4(transformed.xy, 0.0, 1.0);
    }
|};

let fragmentSource =
  {|
    precision mediump float;
    varying vec2 vPosition;
    varying vec2 sdfPos;

    uniform sampler2D tiles;
    uniform sampler2D sdfTiles;

    const int numCols = 12;
    const int numRows = 26;

    void main() {
        vec2 tilePos = vec2((vPosition.x + 1.0) * 0.5, (vPosition.y - 1.0) * -0.5);
        float tile = texture2D(tiles, tilePos).x;
        int colorIdx = int(tile * 255.0);
        vec3 color =
            (colorIdx == 1) ? |}
  ++ Color.toGlsl(Game.colors[2])
  ++ {|
            : (colorIdx == 2) ? |}
  ++ Color.toGlsl(Game.colors[3])
  ++ {|
            : (colorIdx == 3) ? |}
  ++ Color.toGlsl(Game.colors[4])
  ++ {|
            : (colorIdx == 4) ? |}
  ++ Color.toGlsl(Game.colors[5])
  ++ {|
            : (colorIdx == 5) ? |}
  ++ Color.toGlsl(Game.colors[6])
  ++ {|
            : (colorIdx == 6) ? |}
  ++ Color.toGlsl(Game.colors[7])
  ++ {|
            : |}
  ++ Color.toGlsl(Game.colors[8])
  ++ {|;
        vec3 sdfColor = texture2D(sdfTiles, sdfPos).xyz;
        //color = color * 0.5 + ((colorIdx == 0) ? vec3(0.0, 0.0, 0.0) : (sdfColor * 0.5));
        /*
        float sdfCoef = abs(0.5 - sdfColor.x);
        float tileCoef = 1.0 - sdfCoef;
        gl_FragColor = (colorIdx == 0) ? vec4(0.0, 0.0, 0.0, 0.0) : vec4(color * tileCoef + sdfColor * sdfCoef, 1.0);
        */
        gl_FragColor = (colorIdx == 0) ? vec4(0.0, 0.0, 0.0, 0.0) : vec4(mix(color, sdfColor, 0.4), 1.0);
    }
|};

open Gpu;

let makeNode = (tilesTex, sdfTiles) =>
  Scene.(
    Scene.makeNode(
      ~key="tilesDraw",
      ~vertShader=Shader.make(vertexSource),
      ~fragShader=Shader.make(fragmentSource),
      ~uniforms=[],
      ~transparent=true,
      ~partialDraw=true,
      ~textures=[("tiles", tilesTex), ("sdfTiles", SceneTex.node(sdfTiles))],
      ()
    )
  );

let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    uniform mat3 layout;
    uniform mat3 tileShadowsMat;
    uniform mat3 beamsMat;
    uniform mat3 dropMat;
    uniform mat3 gridLightMat;
    varying vec2 vPosition;
    varying vec2 tileShadowsPos;
    varying vec2 beamsPos;
    varying vec2 dropPos;
    varying vec2 gridLightPos;

    uniform vec2 pixelSize;
    // Coords to take off lines on sides
    varying vec2 lineCoords;
    varying vec2 singlePixel;

    varying vec2 coord;
    void main() {
        vPosition = position;
        coord = (position + 1.0) * 0.5;
        singlePixel = 2.0/pixelSize;
        lineCoords = vec2(
            position.x + singlePixel.x,
            position.y
        );
        vec2 transformed = (vec3(position, 1.0) * layout).xy;
        tileShadowsPos = (vec3(position, 1.0) * tileShadowsMat).xy;
        beamsPos = (vec3(position, 1.0) * beamsMat).xy;
        dropPos = (vec3(position, 1.0) * dropMat).xy;
        gridLightPos = (vec3(position, 1.0) * gridLightMat).xy;
        gl_Position = vec4(transformed.xy, 0.0, 1.0);
    }
|};

let fragmentSource = {|
    precision mediump float;
    uniform vec3 bg;
    uniform vec3 lineColor;
    uniform vec2 elPos;
    uniform vec3 elColor;
    uniform vec3 dropColor;
    uniform vec2 pixelSize;
    uniform sampler2D tiles;
    uniform sampler2D tileShadows;
    uniform sampler2D beams;
    uniform sampler2D drop;
    uniform sampler2D gridLight;
    uniform vec4 centerRadius;
    uniform float completedRows;

    varying vec2 vPosition;
    varying vec2 lineCoords;
    varying vec2 singlePixel;
    varying vec2 tileShadowsPos;
    varying vec2 beamsPos;
    varying vec2 dropPos;
    varying vec2 gridLightPos;

    // Normalized to 0.0 - 1.0
    varying vec2 coord;

    const float numCols = 12.0;
    const float numRows = 26.0;
    const vec2 aspect = vec2(numCols / numRows, 1.0);
    const float colSize = 2.0 / numCols;
    const float rowSize = 2.0 / numRows;
    const float wordSize = 1.0;
    const float wordStart = 0.5;
    const float tileWidth = 1.0 / numCols;
    const float tileHeight = 1.0 / numRows;

    float random(vec2 st) {
        return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
    }

    void main() {
        // todo: Calculate in vertex shader?
        vec2 elVec = vPosition - centerRadius.xy;
        vec2 elVecNorm = normalize(elVec);
        float elVecLength = length(elVec);

        float lengthCoef = max(1.5 - elVecLength, 0.0);

        // Roughly light a triangle below element
        float triangleDir = dot(elVecNorm, vec2(0.0, -1.0));
        //float noiseFactor = random(vPosition) * 0.3 + 0.7;
        float triangleLight = smoothstep(0.0, 0.2, triangleDir) * 0.02;
        vec3 lPos = vec3(centerRadius.x, centerRadius.y + 0.15, 0.25);
        vec3 surfaceToLight = lPos - vec3(vPosition.xy, 0.0);
        vec3 lVec = normalize(surfaceToLight);

        vec3 coneDir = normalize(vec3(0.0, -0.8, 0.2));
        float lightToSurfaceAngle = degrees(acos(dot(-lVec, coneDir)));

        float aoi = dot(vec3(0.0, 0.0, 1.0), lVec);
        float attenuation = 1.0 / (1.0 + 100.0 * pow(length(surfaceToLight), 2.0));
        float endDegree = 60.0 + centerRadius.z * 30.0;
        float startDegree = endDegree - 25.0 - centerRadius.z * 20.0;
        attenuation = attenuation * (1.0 - smoothstep(startDegree, endDegree, lightToSurfaceAngle));
        triangleLight = aoi * attenuation;

        // Global pointLight (not using local position)
        vec3 gridLight = (texture2D(gridLight, gridLightPos).xyz * 0.7) + 0.3;

        // Completed rows indication
        vec3 bg2 = mix(bg, bg * 1.1, step(vPosition.y, -1.0 + completedRows * rowSize));

        // Shadow
        float shadow = texture2D(tileShadows, tileShadowsPos).x;
        vec3 color = mix(bg2 * gridLight, vec3(0.0, 0.0, 0.0), shadow * 0.15);

        // Aura light
        /*
        float skew = (vPosition.y - centerRadius.y + 0.15) / 0.8;
        skew = 1.0;
        float auraLight = max(0.15 - length(elVec * centerRadius.wz) * skew, 0.0) * 0.05;
        color = color + elColor * auraLight;
        */

        // Line
        float xblank = (mod(lineCoords.x, colSize) > singlePixel.x) ? 1.0 : 0.0;
        float x3blank = (mod(lineCoords.x - wordStart, wordSize) > singlePixel.x * 2.0) ? 1.0 : 0.0;
        float yblank = (mod(lineCoords.y, rowSize) > singlePixel.y) ? 1.0 : 0.0;
        float lineCoef = (1.0 - xblank * yblank * x3blank) * 0.9 + (1.0 - x3blank) * 0.1;
        color = mix(color, lineColor * gridLight, lineCoef);

        // Beam
        vec3 beam = texture2D(beams, beamsPos).xyz;
        color = mix(color, elColor, (beam.x == 0.0) ? 0.0 : 0.05);

        // Dropbeam
        float dropBeam = texture2D(drop, dropPos).x;
        color = mix(color, dropColor, dropBeam * 0.2);

        // Color + triangleLight
        vec3 tLight = triangleLight * bg2 * elColor;
        tLight = pow(tLight, vec3(1.0/2.2));
        color = color + tLight;
        // Gamma correction
        gl_FragColor = vec4(color, 1.0);
    }
  |};

open Gpu;

let makeNode =
    (
      boardColor,
      lineColor,
      tilesTex,
      tileShadows,
      beamNode,
      dropNode,
      dropColor,
      sdfTiles,
      elState: SceneState.elState,
      centerRadius,
      completedRows
    ) => {
  let gridLight = GridLight.makeNode();
  Scene.
    /*let bg = UVec3f.vals(0.08, 0.12, 0.22);*/
    (
      makeNode(
        ~key="grid",
        /*
         ~margin=Scene.MarginRBLT(
             Scale(0.01),
             Scale(0.002),
             Scale(0.01),
             Scale(0.005)
         ),
         */
        ~vertShader=Shader.make(vertexSource),
        ~fragShader=Shader.make(fragmentSource),
        ~uniforms=[
          ("bg", boardColor),
          ("lineColor", lineColor),
          ("elPos", elState.pos),
          ("elColor", elState.color),
          ("dropColor", dropColor),
          ("centerRadius", centerRadius),
          ("completedRows", completedRows)
        ],
        ~pixelSizeUniform=true,
        ~textures=[
          ("tiles", tilesTex),
          ("tileShadows", SceneTex.node(tileShadows)),
          ("beams", SceneTex.node(beamNode)),
          ("drop", SceneTex.node(dropNode)),
          ("gridLight", SceneTex.node(gridLight))
        ],
        ~deps=[sdfTiles, beamNode, tileShadows, dropNode, gridLight],
        ()
      )
    );
};

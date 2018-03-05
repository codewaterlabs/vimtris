type elState = {
  vo: Scene.sceneVertexObject,
  pos: Scene.sceneUniform,
  elPos: Data.Vec2.t,
  color: Scene.sceneUniform
};

/* Todo: better "page"/scene system */
type sceneLayout =
  | StartScreen
  | GameScreen
  | PauseScreen
  | HelpScreen
  | StartHelp
  | GameOverScreen;

type sceneState = {
  tiles: array(int),
  sceneLayout,
  fontLayout: FontText.FontLayout.t,
  tilesTex: Scene.sceneTexture,
  elState,
  elCenterRadius: Scene.sceneUniform,
  nextEls: array(elState),
  holdingEl: elState,
  beamVO: Scene.sceneVertexObject,
  dropBeamVO: Scene.sceneVertexObject,
  dropColor: Scene.sceneUniform,
  blinkVO: Scene.sceneVertexObject,
  sceneLight: Light.ProgramLight.t,
  elLightPos: Scene.sceneUniform,
  sceneAndElLight: Light.ProgramLight.t,
  bgColor: Scene.sceneUniform,
  boardColor: Scene.sceneUniform,
  lineColor: Scene.sceneUniform,
  mutable gameState: Game.stateT,
  completedRows: Scene.sceneUniform
};

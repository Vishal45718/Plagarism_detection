/**
 * stateManager.js
 * Lightweight reactive state manager — no framework needed.
 */

export const AppState = {
  IDLE: "idle",
  UPLOADING: "uploading",
  ANALYZING: "analyzing",
  COMPLETED: "completed",
  ERROR: "error",
};

let _state = {
  status: AppState.IDLE,
  files: [],
  progress: 0,
  progressLabel: "",
  result: null,
  error: null,
  darkMode: false,
  activeCompareId: null,
  filterRisk: "ALL",
};

const _subscribers = new Set();

export function getState() {
  return { ..._state };
}

export function setState(patch) {
  _state = { ..._state, ...patch };
  _subscribers.forEach((fn) => fn(_state));
}

export function subscribe(fn) {
  _subscribers.add(fn);
  return () => _subscribers.delete(fn);
}

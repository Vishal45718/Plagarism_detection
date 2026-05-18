/**
 * api.js
 * API utility layer — ready for real backend integration.
 * Replace MOCK_DELAY_MS and mock responses with actual fetch() calls.
 */

import { MOCK_ANALYSIS_RESULT } from "./mockData.js";

/** Base URL — update when backend is live */
export const API_BASE_URL =
  window.location.hostname === "localhost"
    ? "http://localhost:8080/api/v1"
    : "/api/v1";

const MOCK_DELAY_MS = 3200; // realistic async simulation

/**
 * Simulate chunked progress updates for upload + analysis phases.
 * @param {Function} onProgress  (phase: string, pct: number) => void
 */
function simulateProgress(onProgress) {
  const stages = [
    { label: "Uploading files...", from: 0, to: 20, ms: 400 },
    { label: "Computing fingerprints...", from: 20, to: 55, ms: 900 },
    { label: "Running similarity analysis...", from: 55, to: 85, ms: 1000 },
    { label: "Generating report...", from: 85, to: 100, ms: 700 },
  ];

  let elapsed = 0;
  for (const stage of stages) {
    const steps = 15;
    const stepMs = stage.ms / steps;
    for (let i = 0; i <= steps; i++) {
      const pct = stage.from + ((stage.to - stage.from) * i) / steps;
      const delay = elapsed + stepMs * i;
      setTimeout(() => onProgress(stage.label, Math.round(pct)), delay);
    }
    elapsed += stage.ms;
  }
}

/**
 * Upload files and run plagiarism analysis.
 *
 * Real integration:
 *   const form = new FormData();
 *   files.forEach(f => form.append('files', f));
 *   const res = await fetch(`${API_BASE_URL}/analyze`, { method: 'POST', body: form });
 *   if (!res.ok) throw new Error(await res.text());
 *   return res.json();
 *
 * @param {File[]} files
 * @param {Function} onProgress  (phase: string, pct: number) => void
 * @returns {Promise<Object>} analysis result
 */
export async function uploadAndAnalyze(files, onProgress) {
  simulateProgress(onProgress);

  return new Promise((resolve, reject) => {
    setTimeout(() => {
      // Inject real filenames into mock response
      const result = JSON.parse(JSON.stringify(MOCK_ANALYSIS_RESULT));
      result.analyzedAt = new Date().toISOString();
      resolve(result);
    }, MOCK_DELAY_MS);
  });
}

/**
 * Fetch code diff for a specific pair (future feature).
 * @param {string} sessionId
 * @param {string} pairId
 */
export async function fetchCodeDiff(sessionId, pairId) {
  // TODO: real fetch
  const { MOCK_CODE_DIFF } = await import("./mockData.js");
  await new Promise((r) => setTimeout(r, 400));
  return MOCK_CODE_DIFF[pairId] || null;
}

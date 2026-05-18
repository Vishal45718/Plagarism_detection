/**
 * app.js — VeriScan Pro main application controller
 * ES-module; imported by index.html as type="module"
 */

import { uploadAndAnalyze, fetchCodeDiff } from "./api.js";
import { AppState, getState, setState, subscribe } from "./stateManager.js";

/* ── Allowed extensions ─────────────────────────────────── */
const ALLOWED_EXT = new Set([".cpp", ".c", ".h", ".py", ".java"]);

function getExt(filename) {
  const m = filename.match(/(\.[^.]+)$/);
  return m ? m[1].toLowerCase() : "";
}

function formatBytes(bytes) {
  if (bytes < 1024) return bytes + " B";
  if (bytes < 1048576) return (bytes / 1024).toFixed(1) + " KB";
  return (bytes / 1048576).toFixed(1) + " MB";
}

/* ── DOM refs ────────────────────────────────────────────── */
const dropZone      = document.getElementById("drop-zone");
const fileInput     = document.getElementById("file-input");
const fileList      = document.getElementById("file-list");
const analyzeBtn    = document.getElementById("analyze-btn");
const cancelBtn     = document.getElementById("cancel-btn");
const progressWrap  = document.getElementById("progress-bar-wrap");
const progressFill  = document.getElementById("progress-fill");
const progressLabel = document.getElementById("progress-label");
const progressPct   = document.getElementById("progress-pct");
const uploadSection = document.getElementById("upload-section");
const resultsSection= document.getElementById("results-section");
const resultsGrid   = document.getElementById("results-grid");
const summaryStrip  = document.getElementById("summary-strip");
const darkToggle    = document.getElementById("dark-toggle");
const darkIcon      = document.getElementById("dark-icon");
const compareModal  = document.getElementById("compare-modal");
const modalClose    = document.getElementById("modal-close");
const modalContent  = document.getElementById("modal-content");
const filterTabs    = document.querySelectorAll(".filter-tab");
const errorBanner   = document.getElementById("error-banner");
const errorMsg      = document.getElementById("error-msg");
const newScanBtn    = document.getElementById("new-scan-btn");

/* ── Dark mode ───────────────────────────────────────────── */
(function initDark() {
  const saved = localStorage.getItem("darkMode");
  if (saved === "true") applyDark(true);
})();

darkToggle.addEventListener("click", () => {
  const { darkMode } = getState();
  applyDark(!darkMode);
});

function applyDark(on) {
  setState({ darkMode: on });
  document.documentElement.classList.toggle("dark", on);
  document.getElementById("navbar").classList.toggle("dark-bg", on);
  document.getElementById("navbar").classList.toggle("light-bg", !on);
  darkIcon.textContent = on ? "light_mode" : "dark_mode";
  localStorage.setItem("darkMode", on);
}

/* ── Drag & Drop ─────────────────────────────────────────── */
dropZone.addEventListener("dragenter", (e) => { e.preventDefault(); dropZone.classList.add("drag-over"); });
dropZone.addEventListener("dragover",  (e) => { e.preventDefault(); dropZone.classList.add("drag-over"); });
dropZone.addEventListener("dragleave", ()  => dropZone.classList.remove("drag-over"));
dropZone.addEventListener("drop", (e) => {
  e.preventDefault();
  dropZone.classList.remove("drag-over");
  handleFiles([...e.dataTransfer.files]);
});

dropZone.addEventListener("click", (e) => {
  if (e.target.classList.contains("chip-remove")) return;
  fileInput.click();
});

fileInput.addEventListener("change", () => handleFiles([...fileInput.files]));

function handleFiles(incoming) {
  const current = getState().files;
  const valid = [], invalid = [];

  incoming.forEach((f) => {
    if (ALLOWED_EXT.has(getExt(f.name))) valid.push(f);
    else invalid.push(f);
  });

  if (invalid.length) {
    dropZone.classList.add("rejected");
    setTimeout(() => dropZone.classList.remove("rejected"), 700);
    showErrorBanner(`Unsupported file type(s): ${invalid.map(f => f.name).join(", ")}. Allowed: .cpp .c .h .py .java`);
  }

  if (valid.length) {
    // Deduplicate by name
    const names = new Set(current.map(f => f.name));
    const merged = [...current, ...valid.filter(f => !names.has(f.name))];
    setState({ files: merged });
    renderFileList(merged);
    hideErrorBanner();
  }
}

function renderFileList(files) {
  fileList.innerHTML = "";
  if (!files.length) { fileList.classList.add("hidden"); return; }
  fileList.classList.remove("hidden");

  files.forEach((f, idx) => {
    const valid = ALLOWED_EXT.has(getExt(f.name));
    const chip = document.createElement("div");
    chip.className = `file-chip ${valid ? "" : "invalid"}`;
    chip.innerHTML = `
      <span class="material-symbols-outlined text-[16px]">${valid ? "code" : "warning"}</span>
      <span class="truncate max-w-[180px]" title="${f.name}">${f.name}</span>
      <span class="text-xs opacity-60">${formatBytes(f.size)}</span>
      <span class="chip-remove material-symbols-outlined text-[16px]" data-idx="${idx}">close</span>
    `;
    chip.querySelector(".chip-remove").addEventListener("click", (e) => {
      e.stopPropagation();
      const files2 = [...getState().files];
      files2.splice(Number(e.target.dataset.idx), 1);
      setState({ files: files2 });
      renderFileList(files2);
    });
    fileList.appendChild(chip);
  });
}

/* ── Analyze button ──────────────────────────────────────── */
analyzeBtn.addEventListener("click", async () => {
  const { files, status } = getState();
  if (status !== AppState.IDLE && status !== AppState.COMPLETED) return;
  if (!files.length) {
    showErrorBanner("Please add at least one source file before analyzing.");
    dropZone.classList.add("rejected");
    setTimeout(() => dropZone.classList.remove("rejected"), 700);
    return;
  }
  hideErrorBanner();
  await runAnalysis(files);
});

cancelBtn.addEventListener("click", () => {
  setState({ status: AppState.IDLE, files: [], result: null, progress: 0, progressLabel: "" });
  renderFileList([]);
  showUploadSection();
  fileInput.value = "";
  hideErrorBanner();
});

async function runAnalysis(files) {
  setState({ status: AppState.UPLOADING });
  showProgress();

  try {
    const result = await uploadAndAnalyze(files, (label, pct) => {
      setState({ progress: pct, progressLabel: label,
        status: pct < 20 ? AppState.UPLOADING : AppState.ANALYZING });
      updateProgressUI(label, pct);
    });
    setState({ status: AppState.COMPLETED, result, progress: 100 });
    updateProgressUI("Analysis complete!", 100);
    setTimeout(() => showResultsSection(result), 500);
  } catch (err) {
    setState({ status: AppState.ERROR, error: err.message });
    showErrorBanner("Analysis failed: " + err.message);
    hideProgress();
  }
}

/* ── Progress UI ─────────────────────────────────────────── */
function showProgress() {
  progressWrap.classList.add("visible");
  analyzeBtn.disabled = true;
  analyzeBtn.classList.add("opacity-60", "cursor-not-allowed");
}
function hideProgress() {
  progressWrap.classList.remove("visible");
  analyzeBtn.disabled = false;
  analyzeBtn.classList.remove("opacity-60", "cursor-not-allowed");
}
function updateProgressUI(label, pct) {
  progressFill.style.width = pct + "%";
  progressLabel.textContent = label;
  progressPct.textContent = pct + "%";
}

/* ── Section toggling ────────────────────────────────────── */
function showUploadSection() {
  uploadSection.classList.remove("hidden-section");
  resultsSection.classList.add("hidden-section");
}
function showResultsSection(result) {
  uploadSection.classList.add("hidden-section");
  resultsSection.classList.remove("hidden-section");
  renderSummary(result);
  renderResults(result.pairs);
  resultsSection.scrollIntoView({ behavior: "smooth", block: "start" });
}

newScanBtn.addEventListener("click", () => {
  setState({ status: AppState.IDLE, files: [], result: null, progress: 0 });
  renderFileList([]);
  hideProgress();
  hideErrorBanner();
  fileInput.value = "";
  showUploadSection();
  updateProgressUI("", 0);
});

/* ── Summary strip ───────────────────────────────────────── */
function renderSummary(result) {
  const pairs = result.pairs;
  const high   = pairs.filter(p => p.riskLevel === "HIGH").length;
  const medium = pairs.filter(p => p.riskLevel === "MEDIUM").length;
  const low    = pairs.filter(p => p.riskLevel === "LOW").length;
  const safe   = pairs.filter(p => p.riskLevel === "SAFE").length;
  const avgJaccard = (pairs.reduce((s,p) => s + p.jaccardSimilarity, 0) / pairs.length * 100).toFixed(1);

  summaryStrip.innerHTML = `
    <div class="stat-box text-center">
      <div class="text-3xl font-bold text-red-500">${high}</div>
      <div class="text-xs font-semibold mt-1 opacity-60 uppercase tracking-wider">High Risk</div>
    </div>
    <div class="stat-box text-center">
      <div class="text-3xl font-bold text-orange-500">${medium}</div>
      <div class="text-xs font-semibold mt-1 opacity-60 uppercase tracking-wider">Medium Risk</div>
    </div>
    <div class="stat-box text-center">
      <div class="text-3xl font-bold text-yellow-500">${low}</div>
      <div class="text-xs font-semibold mt-1 opacity-60 uppercase tracking-wider">Low Risk</div>
    </div>
    <div class="stat-box text-center">
      <div class="text-3xl font-bold text-green-500">${safe}</div>
      <div class="text-xs font-semibold mt-1 opacity-60 uppercase tracking-wider">Safe</div>
    </div>
    <div class="stat-box text-center">
      <div class="text-3xl font-bold text-indigo-500">${avgJaccard}%</div>
      <div class="text-xs font-semibold mt-1 opacity-60 uppercase tracking-wider">Avg Jaccard</div>
    </div>
    <div class="stat-box text-center">
      <div class="text-3xl font-bold text-blue-500">${result.totalPairs}</div>
      <div class="text-xs font-semibold mt-1 opacity-60 uppercase tracking-wider">Pairs Checked</div>
    </div>
  `;
}

/* ── Risk filter tabs ────────────────────────────────────── */
filterTabs.forEach(tab => {
  tab.addEventListener("click", () => {
    filterTabs.forEach(t => t.classList.remove("active"));
    tab.classList.add("active");
    const filter = tab.dataset.filter;
    setState({ filterRisk: filter });
    const { result } = getState();
    if (result) {
      const pairs = filter === "ALL" ? result.pairs : result.pairs.filter(p => p.riskLevel === filter);
      renderResults(pairs);
    }
  });
});

/* ── Result grid ─────────────────────────────────────────── */
function renderResults(pairs) {
  resultsGrid.innerHTML = "";
  if (!pairs.length) {
    resultsGrid.innerHTML = `<div class="col-span-full text-center py-12 opacity-50 text-sm">No pairs match this filter.</div>`;
    return;
  }
  pairs.forEach((pair, i) => {
    const card = buildResultCard(pair);
    card.style.animationDelay = `${i * 0.06}s`;
    resultsGrid.appendChild(card);
  });
}

function riskClass(level) {
  return { HIGH: "high", MEDIUM: "medium", LOW: "low", SAFE: "safe" }[level] || "safe";
}

function buildResultCard(pair) {
  const rc = riskClass(pair.riskLevel);
  const fillClass = `fill-${rc}`;
  const badgeClass = `badge-${rc}`;
  const riskLabel = pair.riskLevel.charAt(0) + pair.riskLevel.slice(1).toLowerCase() + " Risk";
  const jPct  = Math.round(pair.jaccardSimilarity  * 100);
  const coPct = Math.round(pair.cosineSimilarity    * 100);
  const caP   = Math.round(pair.containmentA        * 100);
  const cbP   = Math.round(pair.containmentB        * 100);

  const card = document.createElement("div");
  card.className = "result-card";
  card.dataset.pairId = pair.id;
  card.innerHTML = `
    <div class="flex items-start justify-between gap-2 mb-4">
      <div class="flex-1 min-w-0">
        <div class="flex items-center gap-1 text-xs font-mono opacity-60 mb-1">
          <span class="material-symbols-outlined text-[14px]">insert_drive_file</span>
          <span class="truncate">${pair.file1}</span>
        </div>
        <div class="flex items-center gap-1 text-xs font-mono opacity-60">
          <span class="material-symbols-outlined text-[14px]">insert_drive_file</span>
          <span class="truncate">${pair.file2}</span>
        </div>
      </div>
      <span class="badge ${badgeClass} shrink-0">
        <span class="material-symbols-outlined text-[12px]">
          ${pair.riskLevel === "HIGH" ? "warning" : pair.riskLevel === "SAFE" ? "check_circle" : "info"}
        </span>
        ${riskLabel}
      </span>
    </div>

    <div class="flex flex-col gap-3 mb-4">
      ${metricRow("Jaccard", jPct, fillClass)}
      ${metricRow("Cosine",  coPct, fillClass)}
      ${metricRow("Contain A", caP, fillClass)}
      ${metricRow("Contain B", cbP, fillClass)}
    </div>

    <div class="flex items-center justify-between pt-3 border-t border-outline-variant/30">
      <div class="text-xs opacity-60">
        <span class="font-semibold">${pair.commonHashes}</span> common fingerprints
      </div>
      <button class="compare-btn flex items-center gap-1 text-xs font-semibold text-indigo-500 hover:text-indigo-700 transition-colors" data-pair='${JSON.stringify(pair)}'>
        <span class="material-symbols-outlined text-[15px]">compare</span> Compare
      </button>
    </div>
  `;

  card.querySelector(".compare-btn").addEventListener("click", (e) => {
    e.stopPropagation();
    const p = JSON.parse(e.currentTarget.dataset.pair);
    openCompareModal(p);
  });

  return card;
}

function metricRow(label, pct, fillClass) {
  return `
    <div class="flex items-center gap-3">
      <span class="text-xs opacity-60 w-20 shrink-0">${label}</span>
      <div class="metric-bar-track">
        <div class="metric-bar-fill ${fillClass}" style="width:${pct}%"></div>
      </div>
      <span class="text-xs font-bold w-8 text-right">${pct}%</span>
    </div>
  `;
}

/* ── Compare modal ───────────────────────────────────────── */
async function openCompareModal(pair) {
  compareModal.classList.add("open");
  modalContent.innerHTML = `<div class="flex justify-center py-12"><div class="skeleton w-full h-40 rounded-lg"></div></div>`;

  const diff = await fetchCodeDiff("session", pair.id);
  renderModal(pair, diff);
}

function renderModal(pair, diff) {
  const rc = riskClass(pair.riskLevel);
  modalContent.innerHTML = `
    <div class="p-6 border-b border-outline-variant/30">
      <div class="flex items-center justify-between flex-wrap gap-3">
        <div>
          <h3 class="text-lg font-bold">Side-by-Side Code Comparison</h3>
          <p class="text-sm opacity-60 mt-0.5">${pair.file1} &nbsp;vs&nbsp; ${pair.file2}</p>
        </div>
        <span class="badge badge-${rc}">
          ${pair.riskLevel.charAt(0) + pair.riskLevel.slice(1).toLowerCase()} Risk
        </span>
      </div>
    </div>
    ${diff ? renderDiff(diff, pair) : placeholderDiff(pair)}
    <div class="p-4 border-t border-outline-variant/30 bg-amber-50 dark:bg-amber-900/10 rounded-b-2xl">
      <p class="text-xs text-amber-700 dark:text-amber-400 flex items-center gap-1.5">
        <span class="material-symbols-outlined text-[14px]">construction</span>
        <strong>Coming Soon:</strong> Full highlighted diff with token-level matching powered by the C++ backend.
      </p>
    </div>
  `;
}

function renderDiff(diff, pair) {
  const side = (lines, filename) => `
    <div class="flex-1 min-w-0">
      <div class="px-3 py-2 bg-surface-container/50 dark:bg-white/5 text-xs font-mono font-bold border-b border-outline-variant/20 truncate">
        ${filename}
      </div>
      <div class="diff-panel overflow-x-auto py-2">
        ${lines.map(l => `
          <div class="diff-line ${l.flagged ? "flagged" : ""}">
            <span class="ln">${l.line}</span>
            <span class="code">${l.content.replace(/</g,"&lt;") || "&nbsp;"}</span>
          </div>`).join("")}
      </div>
    </div>
  `;
  return `<div class="flex divide-x divide-outline-variant/20 overflow-hidden">${side(diff.file1Lines, pair.file1)}${side(diff.file2Lines, pair.file2)}</div>`;
}

function placeholderDiff(pair) {
  return `
    <div class="flex flex-col items-center justify-center py-16 gap-4 px-6">
      <span class="material-symbols-outlined text-[48px] opacity-20">code</span>
      <p class="text-center opacity-50 text-sm max-w-xs">
        Highlighted diff for <strong>${pair.file1}</strong> vs <strong>${pair.file2}</strong> will appear here once the backend integration is complete.
      </p>
    </div>
  `;
}

modalClose.addEventListener("click", () => compareModal.classList.remove("open"));
compareModal.addEventListener("click", (e) => { if (e.target === compareModal) compareModal.classList.remove("open"); });

/* ── Error banner ────────────────────────────────────────── */
function showErrorBanner(msg) {
  errorMsg.textContent = msg;
  errorBanner.classList.remove("hidden-section");
}
function hideErrorBanner() {
  errorBanner.classList.add("hidden-section");
}
document.getElementById("error-close").addEventListener("click", hideErrorBanner);

/* ── Init ────────────────────────────────────────────────── */
document.getElementById("navbar").classList.add("light-bg");
showUploadSection();

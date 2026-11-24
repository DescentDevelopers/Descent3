// Upload page logic. Without JS the form still works as a plain multipart POST; this adds
// validation, uploads only the folder containing d3.hog, and shows progress.
(() => {
  'use strict';

  const form = document.getElementById('upload-form');
  const input = document.getElementById('file');
  const picker = document.getElementById('picker');
  const pickerTitle = document.getElementById('picker-title');
  const pickerSub = document.getElementById('picker-sub');
  const progress = document.getElementById('progress');
  const bar = document.getElementById('bar');
  const fill = document.getElementById('fill');
  const progressLabel = document.getElementById('progress-label');
  const progressDetail = document.getElementById('progress-detail');
  const message = document.getElementById('message');
  const submit = document.getElementById('submit');
  const moviesOption = document.getElementById('movies-option');
  const moviesSize = document.getElementById('movies-size');
  const includeMovies = document.getElementById('include-movies');

  const HOG_NAME = 'd3.hog'; // Must match the server's check (case-sensitive).

  // --- Install locations ---------------------------------------------------------------------

  // navigator.userAgentData needs a secure context, which this plain-HTTP page isn't.
  function detectPlatform() {
    const ua = navigator.userAgent;
    if (/Android|iPhone|iPad|CrOS/.test(ua)) return null;
    if (/Windows/.test(ua)) return 'windows';
    if (/Macintosh|Mac OS X/.test(ua)) return 'mac';
    if (/Linux|X11/.test(ua)) return 'linux';
    return null;
  }

  // Shows the visitor's platform first and tucks the others into a disclosure. If the platform is
  // unknown, everything stays listed.
  function arrangeLocations() {
    const section = document.getElementById('locations');
    const platforms = Array.from(section.querySelectorAll('.platform'));
    const mine = platforms.find((p) => p.dataset.platform === detectPlatform());
    if (!mine) return;

    const others = document.createElement('details');
    others.className = 'other-platforms';
    const summary = document.createElement('summary');
    summary.textContent = 'Other platforms';
    others.append(summary);
    for (const p of platforms) {
      if (p !== mine) others.append(p);
    }
    mine.after(others);
  }

  // navigator.clipboard also needs a secure context, so fall back to the legacy copy command.
  async function copyText(text) {
    if (navigator.clipboard && window.isSecureContext) {
      await navigator.clipboard.writeText(text);
      return true;
    }
    const area = document.createElement('textarea');
    area.value = text;
    area.setAttribute('readonly', '');
    area.style.position = 'fixed';
    area.style.opacity = '0';
    document.body.append(area);
    area.select();
    let ok = false;
    try {
      ok = document.execCommand('copy');
    } finally {
      area.remove();
    }
    return ok;
  }

  function addCopyButtons() {
    for (const code of document.querySelectorAll('#locations .path')) {
      const button = document.createElement('button');
      button.type = 'button';
      button.className = 'copy';
      button.textContent = 'Copy';
      button.addEventListener('click', async () => {
        let ok = false;
        try {
          ok = await copyText(code.textContent);
        } catch (e) {
          ok = false;
        }
        if (!ok) {
          // Leave the path selected so it can be copied by hand.
          window.getSelection().selectAllChildren(code);
        }
        button.classList.toggle('done', ok);
        button.textContent =
          ok ? 'Copied' : detectPlatform() === 'mac' ? 'Press ⌘C' : 'Press Ctrl+C';
        clearTimeout(button.resetTimer);
        button.resetTimer = setTimeout(() => {
          button.classList.remove('done');
          button.textContent = 'Copy';
        }, 2000);
      });
      code.after(button);
    }
  }

  arrangeLocations();
  addCopyButtons();

  // --- Upload --------------------------------------------------------------------------------

  let selection = null; // { files, folder, totalBytes }
  let uploading = false;

  function formatBytes(n) {
    const units = ['B', 'KB', 'MB', 'GB'];
    let i = 0;
    while (n >= 1024 && i < units.length - 1) {
      n /= 1024;
      i++;
    }
    return `${n.toFixed(i === 0 ? 0 : 1)} ${units[i]}`;
  }

  function formatDuration(seconds) {
    if (!isFinite(seconds)) return '';
    seconds = Math.max(0, Math.round(seconds));
    const m = Math.floor(seconds / 60);
    const s = seconds % 60;
    return m > 0 ? `${m}m ${String(s).padStart(2, '0')}s` : `${s}s`;
  }

  function showMessage(kind, title, text) {
    message.className = `message ${kind}`;
    message.replaceChildren();
    const strong = document.createElement('strong');
    strong.textContent = title;
    message.append(strong);
    if (text) message.append(text);
    message.hidden = false;
  }

  function hideMessage() {
    message.hidden = true;
  }

  function setProgress(fraction, label, detail) {
    progress.hidden = false;
    bar.classList.toggle('indeterminate', fraction === null);
    fill.style.width = fraction === null ? '' : `${(fraction * 100).toFixed(1)}%`;
    progressLabel.textContent = label;
    progressDetail.textContent = detail || '';
  }

  function setBusy(busy) {
    uploading = busy;
    form.classList.toggle('busy', busy);
    input.disabled = busy;
    includeMovies.disabled = busy;
    submit.disabled = busy || !selection;
  }

  const sumBytes = (files) => files.reduce((sum, f) => sum + f.size, 0);

  // Which files the game uses, relative to the folder holding d3.hog (see USAGE.md): the
  // top-level .hog files, missions/, the top-level .pld controller configs, and optionally
  // movies/. Everything else (executables, DLLs, manuals, Windows-only netgame and online
  // modules, ...) is useless on Android.
  function classify(relPath) {
    const parts = relPath.toLowerCase().split('/');
    if (parts.length === 1) {
      return /\.(hog|pld)$/.test(parts[0]) ? 'required' : 'skipped';
    }
    if (parts[0] === 'missions') return 'required';
    if (parts[0] === 'movies') return 'movies';
    return 'skipped';
  }

  // Finds the shallowest d3.hog in the selection; the server uses its parent folder as the game
  // data root, so only files under that folder are worth uploading.
  function analyze(files) {
    let hog = null;
    let hogDepth = Infinity;
    for (const f of files) {
      const parts = (f.webkitRelativePath || f.name).split('/');
      if (parts[parts.length - 1] === HOG_NAME && parts.length < hogDepth) {
        hog = f;
        hogDepth = parts.length;
      }
    }
    if (!hog) return null;

    const hogPath = hog.webkitRelativePath || hog.name;
    const prefix = hogPath.slice(0, hogPath.length - HOG_NAME.length);
    const groups = { required: [], movies: [], skipped: [] };
    for (const f of files) {
      const path = f.webkitRelativePath || f.name;
      groups[path.startsWith(prefix) ? classify(path.slice(prefix.length)) : 'skipped'].push(f);
    }
    return {
      folder: prefix.replace(/\/$/, '') || '(selected folder)',
      required: groups.required,
      movies: groups.movies,
      skipped: groups.skipped,
    };
  }

  function filesToUpload() {
    return includeMovies.checked ? selection.required.concat(selection.movies) : selection.required;
  }

  function describeSelection() {
    const files = filesToUpload();
    const skipped = files.length === selection.required.length + selection.movies.length
      ? '' : ' (movies skipped)';
    pickerSub.textContent =
      `${files.length} files · ${formatBytes(sumBytes(files))}${skipped} · click to change`;
  }

  includeMovies.addEventListener('change', () => {
    if (selection) describeSelection();
  });

  input.addEventListener('change', () => {
    hideMessage();
    progress.hidden = true;
    const files = Array.from(input.files);
    selection = files.length ? analyze(files) : null;

    moviesOption.hidden = !selection || selection.movies.length === 0;
    if (selection) moviesSize.textContent = `(${formatBytes(sumBytes(selection.movies))})`;

    if (!files.length) {
      picker.classList.remove('selected');
      pickerTitle.textContent = 'Choose folder…';
      pickerSub.textContent = 'Nothing selected yet';
    } else if (!selection) {
      picker.classList.remove('selected');
      pickerTitle.textContent = 'Choose a different folder…';
      pickerSub.textContent = `${files.length} files selected`;
      showMessage('err', `No ${HOG_NAME} in that folder.`,
        'Pick the Descent 3 install folder itself (see “Where to find it” above).');
    } else {
      picker.classList.add('selected');
      pickerTitle.textContent = selection.folder;
      describeSelection();
    }
    setBusy(false);
  });

  function responseText(xhr) {
    const doc = new DOMParser().parseFromString(xhr.responseText || '', 'text/html');
    return (doc.body && doc.body.textContent.trim()) || `HTTP ${xhr.status}`;
  }

  form.addEventListener('submit', (event) => {
    event.preventDefault();
    if (!selection || uploading) return;

    const data = new FormData();
    for (const f of filesToUpload()) {
      // The server recreates the folder structure from each file's relative path.
      data.append('file', f, f.webkitRelativePath || f.name);
    }

    const xhr = new XMLHttpRequest();
    const started = performance.now();
    let sentEverything = false;

    xhr.upload.addEventListener('progress', (e) => {
      if (!e.lengthComputable) {
        setProgress(null, `Uploading… ${formatBytes(e.loaded)}`);
        return;
      }
      const elapsed = (performance.now() - started) / 1000;
      const rate = elapsed > 0 ? e.loaded / elapsed : 0;
      const eta = rate > 0 ? (e.total - e.loaded) / rate : Infinity;
      setProgress(
        e.loaded / e.total,
        `${formatBytes(e.loaded)} of ${formatBytes(e.total)}`,
        rate > 0 ? `${formatBytes(rate)}/s · ${formatDuration(eta)} left` : '');
    });

    xhr.upload.addEventListener('load', () => {
      sentEverything = true;
      setProgress(null, 'Unpacking on your device…', 'This can take a minute');
    });

    xhr.addEventListener('load', () => {
      setBusy(false);
      progress.hidden = true;
      if (xhr.status === 200) {
        picker.hidden = true;
        moviesOption.hidden = true;
        submit.parentElement.hidden = true;
        document.getElementById('locations').hidden = true;
        showMessage('ok', 'Upload complete.',
          'Descent 3 is starting on your device. You can close this page.');
      } else if (xhr.status === 400) {
        showMessage('err', 'The device didn’t find d3.hog in the upload.',
          'Make sure you picked the Descent 3 install folder and try again.');
      } else {
        showMessage('err', 'Upload failed.', responseText(xhr));
      }
    });

    xhr.addEventListener('error', () => {
      setBusy(false);
      progress.hidden = true;
      if (sentEverything) {
        // Everything was sent but the reply never arrived (e.g. the device dropped off the
        // network while unpacking), so the upload may well have succeeded.
        showMessage('ok', 'Upload sent.',
          'The device closed the connection before confirming. If Descent 3 started on your ' +
          'device, you’re all set; otherwise, try again.');
      } else {
        showMessage('err', 'Lost connection to the device.',
          'Check that it’s still on the same network with this page open, then try again.');
      }
    });

    hideMessage();
    setBusy(true);
    setProgress(0, 'Starting upload…');
    xhr.open('POST', '/');
    xhr.send(data);
  });

  window.addEventListener('beforeunload', (event) => {
    if (uploading) event.preventDefault();
  });

  // With JS, the button only enables once a valid folder is chosen.
  setBusy(false);
})();

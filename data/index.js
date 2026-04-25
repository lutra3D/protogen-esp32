    (function () {
      function $(id) { return document.getElementById(id); }
      function setText(node, text) { node.textContent = text; }
      function clamp(value, min, max) { return Math.min(Math.max(value, min), max); }
      function normalizeHex(value, fallback) {
        var safeFallback = (fallback || "#ffffff").toLowerCase();
        return /^#[0-9a-f]{6}$/i.test(value || "") ? value.toUpperCase() : safeFallback.toUpperCase();
      }
      function normalizeUnit(value, fallback) {
        var numeric = Number(value);
        if (!isFinite(numeric)) { return fallback; }
        return clamp(numeric, 0, 1);
      }
      function radiansToUnit(value) {
        var numeric = Number(value);
        if (!isFinite(numeric)) { return 0; }
        return clamp(numeric / (Math.PI * 2), 0, 1);
      }
      function unitToRadians(value) {
        return normalizeUnit(value, 0) * Math.PI * 2;
      }
      function unitToDegrees(value) {
        return normalizeUnit(value, 0) * 360;
      }
      function buildPreviewStyle(emotion) {
        if (emotion && emotion.earColorMode === "gradient") {
          var gradient = emotion.gradient || {};
          var from = normalizeHex(gradient.from, "#44AAFF");
          var to = normalizeHex(gradient.to, "#FF66CC");
          var midpoint = normalizeUnit(gradient.midpoint, 0.5);
          var angle = unitToDegrees(radiansToUnit(gradient.angle));
          var midpointPercent = Math.round(midpoint * 100);
          return "linear-gradient(" + angle + "deg, " + from + " 0%, " + from + " " + midpointPercent + "%, " + to + " 100%)";
        }
        return normalizeHex(emotion && emotion.earColor, "#FFFFFF");
      }

      var uploadForm = $("uploadForm");
      var fileInput = $("fileInput");
      var fileNameInput = $("fileName");
      var uploadStatus = $("uploadStatus");
      var fileList = $("fileList");
      var filesStatus = $("filesStatus");
      var diskUsageLabel = $("diskUsageLabel");
      var diskUsageBar = $("diskUsageBar");
      var emotionCurrent = $("emotionCurrent");
      var emotionStatus = $("emotionStatus");
      var emotionList = $("emotionList");
      var emotionDetail = $("emotionDetail");
      var emotionDetailTitle = $("emotionDetailTitle");
      var emotionForm = $("emotionForm");
      var emotionNameInput = $("emotionNameInput");
      var emotionPathSelect = $("emotionPathSelect");
      var emotionColorMode = $("emotionColorMode");
      var solidControls = $("solidControls");
      var gradientControls = $("gradientControls");
      var emotionSolidColor = $("emotionSolidColor");
      var emotionSolidColorValue = $("emotionSolidColorValue");
      var emotionGradientFrom = $("emotionGradientFrom");
      var emotionGradientFromValue = $("emotionGradientFromValue");
      var emotionGradientTo = $("emotionGradientTo");
      var emotionGradientToValue = $("emotionGradientToValue");
      var emotionGradientAngle = $("emotionGradientAngle");
      var emotionGradientAngleValue = $("emotionGradientAngleValue");
      var emotionGradientMidpoint = $("emotionGradientMidpoint");
      var emotionGradientMidpointValue = $("emotionGradientMidpointValue");
      var emotionGradientPreview = $("emotionGradientPreview");
      var fanDuty = $("fanDuty");
      var fanPercent = $("fanPercent");
      var fanSlider = $("fanSlider");
      var fanSliderValue = $("fanSliderValue");
      var fanStatus = $("fanStatus");
      var earBrightness = $("earBrightness");
      var earBrightnessText = $("earBrightnessText");
      var earBrightnessValue = $("earBrightnessValue");
      var earStatus = $("earStatus");
      var heapValue = $("heapValue");
      var heapStatus = $("heapStatus");
      var gyroValue = $("gyroValue");
      var gyroStatus = $("gyroStatus");

      var allFiles = [];
      var animationFiles = [];
      var emotionDefinitions = [];
      var currentEmotionPath = "";
      var currentEmotionName = "";
      var emotionFormState = {
        mode: "create",
        originalName: "",
        originalPath: ""
      };

      fileInput.addEventListener("change", function () {
        if (fileInput.files.length && !fileNameInput.value) {
          fileNameInput.value = fileInput.files[0].name;
        }
      });

      fanSlider.addEventListener("input", function () {
        setText(fanSliderValue, fanSlider.value + "%");
      });

      earBrightness.addEventListener("input", function () {
        setText(earBrightnessValue, earBrightness.value + "%");
      });

      function fetchText(url, options) {
        return fetch(url, options).then(function (response) {
          return response.text().then(function (text) {
            if (!response.ok) { throw new Error(text || response.statusText); }
            return text;
          });
        });
      }

      function fetchJson(url, options) {
        return fetch(url, options).then(function (response) {
          return response.text().then(function (text) {
            if (!response.ok) { throw new Error(text || response.statusText); }
            if (!text) { return []; }
            try {
              return JSON.parse(text);
            } catch (error) {
              throw new Error("Invalid JSON response");
            }
          });
        });
      }

      function sendEmotionDefinition(method, payload) {
        return fetchText("/emotion", {
          method: method,
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(payload)
        });
      }

      function updateEmotionFormOptions() {
        emotionPathSelect.innerHTML = "";
        if (!animationFiles.length) {
          var emptyOption = document.createElement("option");
          emptyOption.value = "";
          emptyOption.textContent = "No uploaded files available";
          emotionPathSelect.appendChild(emptyOption);
          emotionPathSelect.disabled = true;
          return;
        }

        emotionPathSelect.disabled = false;
        animationFiles.forEach(function (file) {
          var option = document.createElement("option");
          option.value = file.path;
          option.textContent = file.path;
          emotionPathSelect.appendChild(option);
        });
      }

      function getFileNameFromPath(path) {
        var safePath = String(path || "");
        var slashIndex = safePath.lastIndexOf("/");
        return slashIndex >= 0 ? safePath.slice(slashIndex + 1) : safePath;
      }

      function getFileParentPath(path) {
        var safePath = String(path || "");
        var slashIndex = safePath.lastIndexOf("/");
        if (slashIndex <= 0) { return "/"; }
        return safePath.slice(0, slashIndex + 1);
      }

      function canManageAnimFile(path) {
        var safePath = String(path || "");
        return safePath.startsWith("/anim/") || safePath.startsWith("/anims/");
      }

      function applyEmotionModeVisibility() {
        var mode = emotionColorMode.value;
        solidControls.hidden = mode !== "solid";
        gradientControls.hidden = mode !== "gradient";
      }

      function updateGradientPreview() {
        var angleUnit = normalizeUnit(emotionGradientAngle.value, 0);
        var angleRadians = unitToRadians(angleUnit);
        var angleDegrees = unitToDegrees(angleUnit);
        var midpoint = normalizeUnit(emotionGradientMidpoint.value, 0.5);
        var midpointPercent = Math.round(midpoint * 100);
        var from = normalizeHex(emotionGradientFrom.value, "#44AAFF");
        var to = normalizeHex(emotionGradientTo.value, "#FF66CC");

        emotionGradientAngle.value = angleUnit.toFixed(2);
        emotionGradientMidpoint.value = midpoint.toFixed(2);
        emotionGradientFrom.value = from;
        emotionGradientTo.value = to;

        setText(emotionGradientAngleValue, angleUnit.toFixed(2) + " → " + angleRadians.toFixed(2) + " rad");
        setText(emotionGradientMidpointValue, midpoint.toFixed(2));
        setText(emotionGradientFromValue, from);
        setText(emotionGradientToValue, to);
        emotionGradientPreview.style.background = "linear-gradient(" + angleDegrees + "deg, " + from + " 0%, " + from + " " + midpointPercent + "%, " + to + " 100%)";
      }

      function updateSolidColorPreview() {
        var color = normalizeHex(emotionSolidColor.value, "#FFFFFF");
        emotionSolidColor.value = color;
        setText(emotionSolidColorValue, color);
      }

      function getEmotionPayloadFromForm() {
        var mode = emotionColorMode.value === "gradient" ? "gradient" : "solid";
        return {
          name: emotionNameInput.value.trim(),
          path: emotionPathSelect.value,
          earColorMode: mode,
          earColor: normalizeHex(emotionSolidColor.value, "#FFFFFF"),
          gradient: {
            from: normalizeHex(emotionGradientFrom.value, "#44AAFF"),
            to: normalizeHex(emotionGradientTo.value, "#FF66CC"),
            angle: unitToRadians(emotionGradientAngle.value),
            midpoint: normalizeUnit(emotionGradientMidpoint.value, 0.5)
          }
        };
      }

      function hideEmotionForm() {
        emotionDetail.hidden = true;
        emotionForm.reset();
        emotionFormState = { mode: "create", originalName: "", originalPath: "" };
        emotionColorMode.value = "solid";
        emotionSolidColor.value = "#FFFFFF";
        emotionGradientFrom.value = "#44AAFF";
        emotionGradientTo.value = "#FF66CC";
        emotionGradientAngle.value = "0";
        emotionGradientMidpoint.value = "0.5";
        updateEmotionFormOptions();
        applyEmotionModeVisibility();
        updateSolidColorPreview();
        updateGradientPreview();
      }

      function showEmotionForm(mode, emotion) {
        emotionFormState.mode = mode;
        emotionFormState.originalName = emotion && emotion.name ? emotion.name : "";
        emotionFormState.originalPath = emotion && emotion.path ? emotion.path : "";
        emotionDetailTitle.textContent = mode === "edit" ? "Edit emotion" : "New emotion";
        emotionDetail.hidden = false;
        updateEmotionFormOptions();

        if (mode === "edit" && emotion) {
          emotionNameInput.value = emotion.name || "";
          emotionPathSelect.value = emotion.path || animationFiles[0] && animationFiles[0].path || "";
          emotionColorMode.value = emotion.earColorMode === "gradient" ? "gradient" : "solid";
          emotionSolidColor.value = normalizeHex(emotion.earColor, "#FFFFFF");
          emotionGradientFrom.value = normalizeHex(emotion.gradient && emotion.gradient.from, "#44AAFF");
          emotionGradientTo.value = normalizeHex(emotion.gradient && emotion.gradient.to, "#FF66CC");
          emotionGradientAngle.value = String(radiansToUnit(emotion.gradient && emotion.gradient.angle));
          emotionGradientMidpoint.value = String(normalizeUnit(emotion.gradient && emotion.gradient.midpoint, 0.5));
        } else {
          emotionNameInput.value = "";
          emotionPathSelect.value = animationFiles[0] ? animationFiles[0].path : "";
          emotionColorMode.value = "solid";
          emotionSolidColor.value = "#FFFFFF";
          emotionGradientFrom.value = "#44AAFF";
          emotionGradientTo.value = "#FF66CC";
          emotionGradientAngle.value = "0";
          emotionGradientMidpoint.value = "0.5";
        }

        applyEmotionModeVisibility();
        updateSolidColorPreview();
        updateGradientPreview();
        emotionNameInput.focus();
      }

      function renderEmotionList() {
        emotionList.innerHTML = "";
        if (!emotionDefinitions.length) {
          var empty = document.createElement("div");
          empty.className = "emotion-empty";
          empty.textContent = "No emotions configured.";
          emotionList.appendChild(empty);
          return;
        }

        var fragment = document.createDocumentFragment();
        emotionDefinitions.forEach(function (emotion) {
          var card = document.createElement("article");
          card.className = "emotion-card" + ((emotion.path === currentEmotionPath || emotion.name === currentEmotionName) ? " is-active" : "");

          var preview = document.createElement("div");
          preview.className = "emotion-preview";
          preview.style.background = buildPreviewStyle(emotion);

          var body = document.createElement("div");
          body.className = "emotion-body";

          var top = document.createElement("div");
          top.className = "emotion-topline";

          var titleWrap = document.createElement("div");
          var name = document.createElement("div");
          name.className = "emotion-name";
          name.textContent = emotion.name || "(unnamed)";
          var path = document.createElement("div");
          path.className = "emotion-path";
          path.textContent = emotion.path || "--";
          titleWrap.appendChild(name);
          titleWrap.appendChild(path);

          var actions = document.createElement("div");
          actions.className = "emotion-actions";

          var editButton = document.createElement("button");
          editButton.type = "button";
          editButton.className = "icon-button secondary";
          editButton.setAttribute("data-emotion-edit", emotion.name || "");
          editButton.setAttribute("aria-label", "Edit " + (emotion.name || "emotion"));
          editButton.title = "Edit";
          editButton.textContent = "✎";

          var deleteButton = document.createElement("button");
          deleteButton.type = "button";
          deleteButton.className = "icon-button delete-button";
          deleteButton.setAttribute("data-emotion-delete", emotion.name || "");
          deleteButton.setAttribute("aria-label", "Delete " + (emotion.name || "emotion"));
          deleteButton.title = "Delete";
          deleteButton.textContent = "🗑";

          actions.appendChild(editButton);
          actions.appendChild(deleteButton);
          top.appendChild(titleWrap);
          top.appendChild(actions);

          var chips = document.createElement("div");
          chips.className = "emotion-chip-row";

          var modeChip = document.createElement("div");
          modeChip.className = "emotion-chip";
          modeChip.textContent = emotion.earColorMode === "gradient" ? "Gradient" : "Color";

          var colorChip = document.createElement("div");
          colorChip.className = "emotion-chip";
          var swatch = document.createElement("span");
          swatch.className = "color-swatch";
          swatch.style.background = buildPreviewStyle(emotion);
          var chipText = document.createElement("span");
          chipText.textContent = emotion.earColorMode === "gradient"
            ? normalizeHex(emotion.gradient && emotion.gradient.from, "#44AAFF") + " → " + normalizeHex(emotion.gradient && emotion.gradient.to, "#FF66CC")
            : normalizeHex(emotion.earColor, "#FFFFFF");
          colorChip.appendChild(swatch);
          colorChip.appendChild(chipText);

          var activateButton = document.createElement("button");
          activateButton.type = "button";
          activateButton.className = emotion.path === currentEmotionPath || emotion.name === currentEmotionName ? "emotion-activate" : "emotion-activate secondary";
          activateButton.setAttribute("data-emotion-activate", emotion.name || emotion.path || "");
          activateButton.textContent = emotion.path === currentEmotionPath || emotion.name === currentEmotionName ? "Current emotion" : "Set current";

          chips.appendChild(modeChip);
          chips.appendChild(colorChip);
          body.appendChild(top);
          body.appendChild(chips);
          body.appendChild(activateButton);
          card.appendChild(preview);
          card.appendChild(body);
          fragment.appendChild(card);
        });

        emotionList.appendChild(fragment);
      }

      function refreshEmotionDefinitions(options) {
        var preserveStatus = options && options.preserveStatus;
        return fetchJson("/emotions").then(function (data) {
          emotionDefinitions = Array.isArray(data) ? data : [];
          renderEmotionList();
          if (!preserveStatus) {
            setText(emotionStatus, "");
          }
          return emotionDefinitions;
        }).catch(function (error) {
          emotionDefinitions = [];
          renderEmotionList();
          if (!preserveStatus) {
            setText(emotionStatus, "Error: " + error.message);
          }
          return [];
        });
      }

      function refreshEmotion(options) {
        var preserveStatus = options && options.preserveStatus;
        return fetchJson("/emotion").then(function (data) {
          var payload = (data && typeof data === "object") ? data : {};
          currentEmotionPath = (payload.path || "").trim();
          currentEmotionName = (payload.name || "").trim();
          setText(emotionCurrent, currentEmotionName || currentEmotionPath || "(empty)");
          renderEmotionList();
          if (!preserveStatus) {
            setText(emotionStatus, "");
          }
          return payload;
        }).catch(function (error) {
          currentEmotionPath = "";
          currentEmotionName = "";
          setText(emotionCurrent, "--");
          renderEmotionList();
          if (!preserveStatus) {
            setText(emotionStatus, "Error: " + error.message);
          }
          return null;
        });
      }

      function setCurrentEmotion(name) {
        var body = new URLSearchParams({ name: name });
        return fetchText("/emotion/current", {
          method: "PUT",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: body
        }).then(function (text) {
          setText(emotionStatus, text);
          return refreshEmotion({ preserveStatus: true }).then(function () {
            renderEmotionList();
            return text;
          });
        }).catch(function (error) {
          setText(emotionStatus, "Error: " + error.message);
          throw error;
        });
      }

      function saveEmotion(event) {
        event.preventDefault();
        if (!animationFiles.length) {
          setText(emotionStatus, "Upload at least one animation file before saving an emotion.");
          return;
        }

        var payload = getEmotionPayloadFromForm();
        if (!payload.name) {
          setText(emotionStatus, "Emotion name is required.");
          emotionNameInput.focus();
          return;
        }
        if (!payload.path) {
          setText(emotionStatus, "Animation file is required.");
          emotionPathSelect.focus();
          return;
        }

        setText(emotionStatus, emotionFormState.mode === "edit" ? "Saving emotion..." : "Creating emotion...");

        if (emotionFormState.mode === "edit") {
          var originalName = emotionFormState.originalName;
          var originalPath = emotionFormState.originalPath;
          var changedName = payload.name !== originalName;
          var changedPath = payload.path !== originalPath;

          if (changedName && changedPath) {
            var wasCurrentEmotion = currentEmotionName === originalName || currentEmotionPath === originalPath;
            sendEmotionDefinition("POST", payload).then(function (text) {
              return fetchText("/emotion?name=" + encodeURIComponent(originalName), { method: "DELETE" }).then(function () {
                return text;
              });
            }).then(function (text) {
              setText(emotionStatus, text + " Recreated with updated name and file path.");
              hideEmotionForm();
              return Promise.all([
                refreshEmotionDefinitions({ preserveStatus: true }),
                refreshEmotion({ preserveStatus: true })
              ]);
            }).then(function () {
              if (wasCurrentEmotion) {
                return setCurrentEmotion(payload.name);
              }
              renderEmotionList();
              return null;
            }).catch(function (error) {
              setText(emotionStatus, "Error: " + error.message);
            });
            return;
          }

          sendEmotionDefinition("PUT", payload).then(function (text) {
            setText(emotionStatus, text);
            hideEmotionForm();
            return Promise.all([
              refreshEmotionDefinitions({ preserveStatus: true }),
              refreshEmotion({ preserveStatus: true })
            ]);
          }).then(function () {
            renderEmotionList();
          }).catch(function (error) {
            setText(emotionStatus, "Error: " + error.message);
          });
          return;
        }

        sendEmotionDefinition("POST", payload).then(function (text) {
          setText(emotionStatus, text);
          hideEmotionForm();
          return Promise.all([
            refreshEmotionDefinitions({ preserveStatus: true }),
            refreshEmotion({ preserveStatus: true })
          ]);
        }).then(function () {
          renderEmotionList();
        }).catch(function (error) {
          setText(emotionStatus, "Error: " + error.message);
        });
      }

      function deleteEmotion(name) {
        if (!name) { return; }
        setText(emotionStatus, "Deleting emotion...");
        fetchText("/emotion?name=" + encodeURIComponent(name), { method: "DELETE" }).then(function (text) {
          setText(emotionStatus, text);
          hideEmotionForm();
          return Promise.all([
            refreshEmotionDefinitions({ preserveStatus: true }),
            refreshEmotion({ preserveStatus: true })
          ]);
        }).then(function () {
          renderEmotionList();
        }).catch(function (error) {
          setText(emotionStatus, "Error: " + error.message);
        });
      }

      function refreshFan() {
        fetchText("/fan").then(function (dutyString) {
          var duty = parseInt(dutyString, 10);
          var percent = isFinite(duty) ? Math.round(duty * 100 / 255) : NaN;
          setText(fanDuty, isFinite(duty) ? duty : "--");
          setText(fanPercent, isFinite(percent) ? percent : "--");
          if (isFinite(percent)) {
            fanSlider.value = percent;
            setText(fanSliderValue, percent + "%");
          }
          setText(fanStatus, "");
        }).catch(function (error) {
          setText(fanStatus, "Error: " + error.message);
        });
      }

      function applyFan() {
        var percent = parseInt(fanSlider.value, 10) || 0;
        var duty = Math.round(percent * 255 / 100);
        var body = new URLSearchParams({ duty: String(duty) });
        fetchText("/fan", {
          method: "PUT",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: body
        }).then(function (text) {
          setText(fanStatus, text);
          refreshFan();
        }).catch(function (error) {
          setText(fanStatus, "Error: " + error.message);
        });
      }

      function refreshEars() {
        fetchJson("/ears").then(function (data) {
          var payload = (data && typeof data === "object" && !Array.isArray(data)) ? data : {};
          var brightnessPercent = Number(payload.brightnessPercent);
          if (isFinite(brightnessPercent)) {
            var rounded = Math.round(brightnessPercent);
            setText(earBrightnessText, rounded + "%");
            earBrightness.value = rounded;
            setText(earBrightnessValue, rounded + "%");
          } else {
            setText(earBrightnessText, "--%");
            setText(earBrightnessValue, "--%");
          }
          setText(earStatus, "");
        }).catch(function (error) {
          setText(earStatus, "Error: " + error.message);
        });
      }

      function applyEars() {
        var body = new URLSearchParams({
          brightnessPercent: earBrightness.value
        });
        fetchText("/ears", {
          method: "PUT",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: body
        }).then(function (text) {
          setText(earStatus, text);
          refreshEars();
        }).catch(function (error) {
          setText(earStatus, "Error: " + error.message);
        });
      }

      function refreshHeap() {
        fetchText("/heap").then(function (text) {
          setText(heapValue, text);
          setText(heapStatus, "");
        }).catch(function (error) {
          setText(heapValue, "--");
          setText(heapStatus, "Error: " + error.message);
        });
      }

      function refreshGyro() {
        fetchText("/gyro").then(function (text) {
          setText(gyroValue, text);
          setText(gyroStatus, "");
        }).catch(function (error) {
          setText(gyroValue, "--");
          setText(gyroStatus, "Error: " + error.message);
        });
      }

      uploadForm.addEventListener("submit", function (event) {
        event.preventDefault();
        if (!fileInput.files.length) {
          setText(uploadStatus, "Please choose a file.");
          return;
        }
        var file = fileInput.files[0];
        var name = fileNameInput.value.trim() || file.name;
        if (!name) {
          setText(uploadStatus, "File name is required.");
          return;
        }
        setText(uploadStatus, "Uploading...");

        var formData = new FormData();
        formData.append("file", file, name);
        fetchText("/file", {
          method: "POST",
          body: formData
        }).then(function (text) {
          setText(uploadStatus, text);
          refreshFiles();
        }).catch(function (error) {
          setText(uploadStatus, "Error: " + error.message);
        });
      });

      function renderFileList(files) {
        fileList.innerHTML = "";
        if (!files.length) {
          var empty = document.createElement("div");
          empty.className = "file-item empty";
          empty.textContent = "No files found.";
          fileList.appendChild(empty);
          return;
        }

        var fragment = document.createDocumentFragment();
        files.forEach(function (file) {
          var item = document.createElement("div");
          item.className = "file-item";

          var label = document.createElement("span");
          label.className = "file-name";
          label.textContent = file.path;

          var actions = document.createElement("div");
          actions.className = "row";
          actions.style.marginTop = "0";

          var downloadButton = document.createElement("button");
          downloadButton.type = "button";
          downloadButton.className = "icon-button secondary";
          downloadButton.setAttribute("data-file-download", file.path);
          downloadButton.setAttribute("aria-label", "Download " + file.path);
          downloadButton.title = "Download";
          downloadButton.textContent = "↓";

          actions.appendChild(downloadButton);

          if (canManageAnimFile(file.path)) {
            var renameButton = document.createElement("button");
            renameButton.type = "button";
            renameButton.className = "icon-button secondary";
            renameButton.setAttribute("data-file-rename", file.path);
            renameButton.setAttribute("aria-label", "Rename " + file.path);
            renameButton.title = "Rename";
            renameButton.textContent = "✎";

            var deleteButton = document.createElement("button");
            deleteButton.type = "button";
            deleteButton.className = "icon-button delete-button";
            deleteButton.setAttribute("data-file-delete", file.path);
            deleteButton.setAttribute("aria-label", "Delete " + file.path);
            deleteButton.title = "Delete";
            deleteButton.textContent = "🗑";

            actions.appendChild(renameButton);
            actions.appendChild(deleteButton);
          }

          item.appendChild(label);
          item.appendChild(actions);
          fragment.appendChild(item);
        });

        fileList.appendChild(fragment);
      }

      function refreshFiles() {
        return fetchJson("/files").then(function (files) {
          allFiles = Array.isArray(files) ? files : [];
          animationFiles = allFiles.filter(function (file) {
            var filePath = (file && file.path) ? String(file.path) : "";
            return filePath.startsWith("/anims/") && filePath.toLowerCase().endsWith(".gif");
          });
          renderFileList(allFiles);
          updateEmotionFormOptions();
          setText(filesStatus, "");
          refreshFilesInfo();
          return allFiles;
        }).catch(function (error) {
          allFiles = [];
          animationFiles = [];
          renderFileList([]);
          updateEmotionFormOptions();
          setText(filesStatus, "Error: " + error.message);
          refreshFilesInfo();
          if (!emotionStatus.textContent) {
            setText(emotionStatus, "Error loading animation files: " + error.message);
          }
          return [];
        });
      }

      function refreshFilesInfo() {
        return fetchJson("/files-info").then(function (info) {
          var payload = (info && typeof info === "object" && !Array.isArray(info)) ? info : {};
          var usedKb = Number(payload.usedSizeBytes)/1024;
          var availableKb = Number(payload.totalSizeBytes)/1024;
          var percent = Number(payload.usedPercentage);
          if (!isFinite(availableKb)) {
            availableKb = Number(payload.availableKb);
          }

          if (!isFinite(usedKb) || !isFinite(availableKb) || availableKb <= 0) {
            throw new Error("Invalid files-info response.");
          }

          diskUsageBar.max = 100;
          diskUsageBar.value = percent;
          setText(diskUsageLabel, "💽 Used: " + usedKb + " KB/ " + availableKb + " KB");
        }).catch(function (error) {
          diskUsageBar.max = 100;
          diskUsageBar.value = 0;
          setText(diskUsageLabel, "💽 Used: -- KB/ -- KB");
          if (!filesStatus.textContent) {
            setText(filesStatus, "Error: " + error.message);
          }
        });
      }

      function deleteFile(name) {
        if (!canManageAnimFile(name)) {
          setText(filesStatus, "Only files in /anim/ can be deleted.");
          return;
        }
        fetchText("/file?file=" + encodeURIComponent(name), { method: "DELETE" })
          .then(function (text) {
            setText(filesStatus, text);
            return refreshFiles();
          })
          .then(function () {
            if (emotionPathSelect.value === name) {
              emotionPathSelect.value = animationFiles[0] ? animationFiles[0].path : "";
            }
          })
          .catch(function (error) {
            setText(filesStatus, "Error: " + error.message);
          });
      }

      function downloadFile(path) {
        var filePath = String(path || "");
        if (!filePath) { return; }
        var link = document.createElement("a");
        link.href = "/file?file=" + encodeURIComponent(filePath);
        link.download = getFileNameFromPath(filePath) || "download.bin";
        document.body.appendChild(link);
        link.click();
        link.remove();
      }

      function renameFile(path) {
        if (!canManageAnimFile(path)) {
          setText(filesStatus, "Only files in /anim/ can be renamed.");
          return;
        }

        var currentName = getFileNameFromPath(path);
        var newName = window.prompt("Enter new file name (name only):", currentName);
        if (newName === null) { return; }

        var trimmedName = newName.trim();
        if (!trimmedName) {
          setText(filesStatus, "New file name is required.");
          return;
        }
        if (trimmedName.indexOf("/") >= 0 || trimmedName.indexOf("\\") >= 0) {
          setText(filesStatus, "Only the filename can be changed (no path separators).");
          return;
        }

        var newPath = getFileParentPath(path) + trimmedName;
        if (newPath === path) {
          setText(filesStatus, "File name is unchanged.");
          return;
        }

        setText(filesStatus, "Renaming file...");
        fetch("/file?file=" + encodeURIComponent(path)).then(function (response) {
          if (!response.ok) { throw new Error("Failed to read original file."); }
          return response.blob();
        }).then(function (blob) {
          var formData = new FormData();
          formData.append("file", blob, newPath);
          return fetchText("/file", { method: "POST", body: formData });
        }).then(function () {
          return fetchText("/file?file=" + encodeURIComponent(path), { method: "DELETE" });
        }).then(function () {
          setText(filesStatus, "File was renamed.");
          return refreshFiles();
        }).catch(function (error) {
          setText(filesStatus, "Error: " + error.message);
        });
      }

      fileList.addEventListener("click", function (event) {
        var downloadButton = event.target.closest("[data-file-download]");
        if (downloadButton) {
          var downloadPath = downloadButton.getAttribute("data-file-download") || "";
          if (downloadPath) {
            downloadFile(downloadPath);
          }
          return;
        }

        var renameButton = event.target.closest("[data-file-rename]");
        if (renameButton) {
          var renamePath = renameButton.getAttribute("data-file-rename") || "";
          if (renamePath) {
            renameFile(renamePath);
          }
          return;
        }

        var button = event.target.closest("[data-file-delete]");
        if (!button) { return; }
        var name = button.getAttribute("data-file-delete") || "";
        if (name) {
          deleteFile(name);
        }
      });

      $("emotionAdd").addEventListener("click", function () {
        showEmotionForm("create");
      });
      $("emotionRefresh").addEventListener("click", function () {
        Promise.all([
          refreshFiles(),
          refreshEmotionDefinitions({ preserveStatus: true }),
          refreshEmotion({ preserveStatus: true })
        ]).then(function () {
          renderEmotionList();
          setText(emotionStatus, "");
        }).catch(function () {
          // Individual loaders already surface errors.
        });
      });
      $("emotionCancel").addEventListener("click", hideEmotionForm);
      emotionForm.addEventListener("submit", saveEmotion);
      emotionColorMode.addEventListener("change", applyEmotionModeVisibility);
      emotionSolidColor.addEventListener("input", updateSolidColorPreview);
      emotionGradientFrom.addEventListener("input", updateGradientPreview);
      emotionGradientTo.addEventListener("input", updateGradientPreview);
      emotionGradientAngle.addEventListener("input", updateGradientPreview);
      emotionGradientMidpoint.addEventListener("input", updateGradientPreview);

      emotionList.addEventListener("click", function (event) {
        var activateButton = event.target.closest("[data-emotion-activate]");
        if (activateButton) {
          var emotionName = activateButton.getAttribute("data-emotion-activate") || "";
          if (emotionName) {
            setText(emotionStatus, "Updating emotion...");
            setCurrentEmotion(emotionName);
          }
          return;
        }

        var editButton = event.target.closest("[data-emotion-edit]");
        if (editButton) {
          var editName = editButton.getAttribute("data-emotion-edit") || "";
          var emotion = emotionDefinitions.find(function (item) { return item.name === editName; });
          if (emotion) {
            showEmotionForm("edit", emotion);
          }
          return;
        }

        var deleteButton = event.target.closest("[data-emotion-delete]");
        if (deleteButton) {
          var deleteName = deleteButton.getAttribute("data-emotion-delete") || "";
          if (deleteName) {
            deleteEmotion(deleteName);
          }
        }
      });

      $("fanRefresh").addEventListener("click", refreshFan);
      $("fanApply").addEventListener("click", applyFan);
      $("earRefresh").addEventListener("click", refreshEars);
      $("earApply").addEventListener("click", applyEars);
      $("heapRefresh").addEventListener("click", refreshHeap);
      $("gyroRefresh").addEventListener("click", refreshGyro);

      hideEmotionForm();
      Promise.all([
        refreshFiles(),
        refreshEmotionDefinitions({ preserveStatus: true }),
        refreshEmotion({ preserveStatus: true })
      ]).then(function () {
        renderEmotionList();
        setText(emotionStatus, "");
      });
      refreshFan();
      refreshEars();
      refreshHeap();
      refreshGyro();
    })();
  

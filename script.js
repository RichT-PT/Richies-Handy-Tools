(function () {
  var CUP_ML = 236.588;
  var STORAGE_KEY = 'syrupCalcState_v1';

  var state = {
    mode: 'food',
    R: 0.77,
    gramsPerCup: 200,
    inputs: {
      food: { value: 4, unit: 'cups' },
      syrup: { value: 2, unit: 'cups' },
      sugar: { value: 1, unit: 'cups' }
    }
  };

  function loadState() {
    try {
      var raw = localStorage.getItem(STORAGE_KEY);
      if (!raw) return;
      var saved = JSON.parse(raw);
      if (saved && typeof saved === 'object') {
        if (typeof saved.mode === 'string') state.mode = saved.mode;
        if (typeof saved.R === 'number' && saved.R > 0) state.R = saved.R;
        if (typeof saved.gramsPerCup === 'number' && saved.gramsPerCup > 0) state.gramsPerCup = saved.gramsPerCup;
        if (saved.inputs) {
          ['food', 'syrup', 'sugar'].forEach(function (m) {
            if (saved.inputs[m] && typeof saved.inputs[m].value === 'number') {
              state.inputs[m].value = saved.inputs[m].value;
            }
            if (saved.inputs[m] && typeof saved.inputs[m].unit === 'string') {
              state.inputs[m].unit = saved.inputs[m].unit;
            }
          });
        }
      }
    } catch (e) { /* storage unavailable, use defaults */ }
  }

  function saveState() {
    try {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(state));
    } catch (e) { /* ignore */ }
  }

  var MODE_CONFIG = {
    food: {
      label: 'Batch of finished food',
      help: 'Enter how much finished hummingbird food (or nectar) you need. The calculator fills in the sugar, syrup, and both waters to make exactly that much.',
      units: [{ key: 'cups', label: 'cups' }, { key: 'ml', label: 'mL' }],
      inputCell: 'food'
    },
    syrup: {
      label: 'Leftover syrup on hand',
      help: 'Enter how much concentrated syrup you already have. The calculator tells you exactly how much water to stir in, and how much food that makes.',
      units: [{ key: 'cups', label: 'cups' }, { key: 'ml', label: 'mL' }],
      inputCell: 'syrup'
    },
    sugar: {
      label: 'Sugar you want to use',
      help: 'Enter how much dry granulated sugar you\'re starting with. The calculator scales the water for syrup, the syrup yield, the dilution water, and the finished food.',
      units: [{ key: 'cups', label: 'cups' }, { key: 'g', label: 'grams' }],
      inputCell: 'sugarCups'
    }
  };

  var el = {
    modeButtons: document.querySelectorAll('.mode-btn'),
    modeHelp: document.getElementById('modeHelp'),
    inputLabel: document.getElementById('inputLabel'),
    primaryInput: document.getElementById('primaryInput'),
    unitToggle: document.getElementById('unitToggle'),
    reductionInput: document.getElementById('reductionInput'),
    gramsInput: document.getElementById('gramsInput'),
    cells: {
      sugarCups: document.querySelector('[data-cell="sugarCups"]'),
      sugarGrams: document.querySelector('[data-cell="sugarGrams"]'),
      waterSyrup: document.querySelector('[data-cell="waterSyrup"]'),
      dilution: document.querySelector('[data-cell="dilution"]'),
      syrup: document.querySelector('[data-cell="syrup"]'),
      food: document.querySelector('[data-cell="food"]')
    }
  };

  function toCups(value, unit) {
    if (unit === 'ml') return value / CUP_ML;
    if (unit === 'g') return value / state.gramsPerCup;
    return value; // cups
  }

  function computeFromSugarCups(S) {
    var waterSyrupCups = S;
    var syrupCups = 2 * S * state.R;
    var dilutionCups = 3 * S;
    var foodCups = syrupCups + dilutionCups;
    return {
      sugarCups: S,
      sugarGrams: S * state.gramsPerCup,
      waterSyrupCups: waterSyrupCups,
      dilutionCups: dilutionCups,
      syrupCups: syrupCups,
      foodCups: foodCups
    };
  }

  function compute() {
    var cfg = MODE_CONFIG[state.mode];
    var raw = state.inputs[state.mode];
    var value = typeof raw.value === 'number' && !isNaN(raw.value) ? raw.value : 0;
    var unit = raw.unit;
    var S;

    if (state.mode === 'food') {
      var F = toCups(value, unit === 'ml' ? 'ml' : 'cups');
      S = (2 * state.R + 3) > 0 ? F / (2 * state.R + 3) : 0;
    } else if (state.mode === 'syrup') {
      var V = toCups(value, unit === 'ml' ? 'ml' : 'cups');
      S = state.R > 0 ? V / (2 * state.R) : 0;
    } else {
      S = toCups(value, unit === 'g' ? 'g' : 'cups');
    }
    if (!isFinite(S) || S < 0) S = 0;
    return computeFromSugarCups(S);
  }

  function fmt(n, decimals) {
    if (!isFinite(n)) return '0';
    var f = n.toFixed(decimals);
    return f;
  }

  function render(keepInput) {
    var cfg = MODE_CONFIG[state.mode];

    el.modeButtons.forEach(function (btn) {
      btn.classList.toggle('active', btn.dataset.mode === state.mode);
    });
    el.modeHelp.textContent = cfg.help;
    el.inputLabel.textContent = cfg.label;

    // unit toggle buttons
    el.unitToggle.innerHTML = '';
    cfg.units.forEach(function (u) {
      var b = document.createElement('button');
      b.type = 'button';
      b.textContent = u.label;
      b.dataset.unit = u.key;
      if (state.inputs[state.mode].unit === u.key) b.classList.add('active');
      b.addEventListener('click', function () {
        state.inputs[state.mode].unit = u.key;
        saveState();
        render();
      });
      el.unitToggle.appendChild(b);
    });

    if (!keepInput) {
  el.primaryInput.value = state.inputs[state.mode].value;
}

    var r = compute();

    // reset all cell "input" highlighting
    Object.keys(el.cells).forEach(function (key) {
      el.cells[key].classList.remove('is-input');
      el.cells[key].querySelector('.tag').hidden = true;
    });

    function setDual(cellKey, cups, mlFactor) {
      var cell = el.cells[cellKey];
      var mlVal = cups * CUP_ML;
      cell.querySelector('.n1').textContent = fmt(cups, cups < 10 ? 2 : 1);
      cell.querySelector('.n2').textContent = fmt(mlVal, 0);
    }

    el.cells.sugarCups.querySelector('.n').textContent = fmt(r.sugarCups, r.sugarCups < 10 ? 2 : 1);
    el.cells.sugarGrams.querySelector('.n').textContent = fmt(r.sugarGrams, 0);
    setDual('waterSyrup', r.waterSyrupCups);
    setDual('dilution', r.dilutionCups);
    setDual('syrup', r.syrupCups);
    setDual('food', r.foodCups);

    // mark the active input cell
    var inputCellKey = cfg.inputCell;
    if (el.cells[inputCellKey]) {
      el.cells[inputCellKey].classList.add('is-input');
      el.cells[inputCellKey].querySelector('.tag').hidden = false;
    }

    el.reductionInput.value = state.R;
    el.gramsInput.value = state.gramsPerCup;
  }

  el.modeButtons.forEach(function (btn) {
    btn.addEventListener('click', function () {
      state.mode = btn.dataset.mode;
      saveState();
      render();
    });
  });

  el.primaryInput.addEventListener('focus', function () {
  el.primaryInput.value = '';
});

el.primaryInput.addEventListener('input', function () {
  var text = el.primaryInput.value.trim();

  // Let the user temporarily leave it blank or type a decimal point.
  if (text === '' || text === '.') {
    return;
  }

  var v = parseFloat(text);

  if (!isNaN(v) && v >= 0) {
    state.inputs[state.mode].value = v;
    saveState();
    render(true);
  }
});

el.primaryInput.addEventListener('blur', function () {
  var text = el.primaryInput.value.trim();

  // If nothing was entered, put the previous value back.
  if (text === '' || text === '.') {
    render();
  }
});

  el.reductionInput.addEventListener('input', function () {
    var v = parseFloat(el.reductionInput.value);
    if (!isNaN(v) && v > 0) {
      state.R = v;
      saveState();
      render();
    }
  });

  el.gramsInput.addEventListener('input', function () {
    var v = parseFloat(el.gramsInput.value);
    if (!isNaN(v) && v > 0) {
      state.gramsPerCup = v;
      saveState();
      render();
    }
  });

  loadState();
  render();
})();

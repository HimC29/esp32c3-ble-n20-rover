const keys = {
    forward: false,
    backward: false,
    left: false,
    right: false
};

const forwardBtn = document.getElementById("forwardBtn");
const backwardBtn = document.getElementById("backwardBtn");
const leftBtn = document.getElementById("leftBtn");
const rightBtn = document.getElementById("rightBtn");

let characteristic;

connectBtn.addEventListener('click', async () => {
    const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: ['91fb9ba5-7485-4dc1-9bac-a9889bc524b9'] }]
    });

    const server = await device.gatt.connect();
    const service = await server.getPrimaryService('91fb9ba5-7485-4dc1-9bac-a9889bc524b9');
    characteristic = await service.getCharacteristic('e942f3ad-99c0-48a0-94ad-43792af38a88');
});

function pressKey(key, el) {
    keys[key] = true;
    el.classList.add('active');
}

function releaseKey(key, el) {
    keys[key] = false;
    el.classList.remove('active');
}

const SPEED = 255;

function buildCommand() {
    let leftDir = 0, leftSpeed = 0, rightDir = 0, rightSpeed = 0;

    if (keys.forward && keys.backward) {
        leftSpeed = 0;
        rightSpeed = 0;
    }
    else if (keys.forward) {
        leftDir = 1;
        rightDir = 1;
        leftSpeed = SPEED;
        rightSpeed = SPEED;
    }
    else if (keys.backward) {
        leftDir = 0;
        rightDir = 0;
        leftSpeed = SPEED;
        rightSpeed = SPEED;
    }

    if (keys.left) {
        leftDir = 0;
        rightDir = 1;
        leftSpeed = SPEED;
        rightSpeed = SPEED;
    }
    if (keys.right) {
        leftDir = 1;
        rightDir = 0;
        leftSpeed = SPEED;
        rightSpeed = SPEED;
    }
    
    return new Uint8Array([leftDir, leftSpeed, rightDir, rightSpeed]);
}

document.querySelectorAll('#controls button').forEach(btn => {
    const key = btn.dataset.key;
    btn.addEventListener('mousedown', () => pressKey(key, btn));
    btn.addEventListener('mouseup', () => releaseKey(key, btn));
    btn.addEventListener('touchstart', e => { e.preventDefault(); pressKey(key, btn); });
    btn.addEventListener('touchend', () => releaseKey(key, btn));
});

setInterval(async () => {
    if (!characteristic) return;
    const command = buildCommand();
    await characteristic.writeValue(command);
}, 100);

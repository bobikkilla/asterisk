#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;

#define PI 3.1415926

// === Геометрия звезды ===
bool points_line_side(vec2 pos, vec2 p0, vec2 v0, vec2 v1) {
    return (pos.x - v0.x) * (v1.y - v0.y) < (pos.y - v0.y) * (v1.x - v0.x) &&
           (p0.x - v0.x) * (v1.y - v0.y) < (p0.y - v0.y) * (v1.x - v0.x);
}

bool point_outside(vec2 pos, vec2 p0, vec2 v0, vec2 v1, vec2 v2) {
    if (!points_line_side(pos, p0, v0, v1) && !points_line_side(pos, p0, v1, v2))
        return true;
    return false;
}

vec4 Star(vec2 pos, vec2 p0, float r, float R, float angle, vec4 color, vec4 bg) {
    const int N = 5;
    vec4 result = bg;

    if (length(pos - p0) < R) {
        result = color;
        for (int i = 0; i < N; ++i) {
            vec2 v[3];
            for (int j = 0; j < 3; ++j) {
                int index = i * 2 + j;
                float a = angle + float(index) * PI / float(N);
                float rad = (mod(float(index), 2.0) < 0.5 ? R : r);
                v[j] = p0 + vec2(cos(a), sin(a)) * rad;
            }
            if (point_outside(pos, p0, v[0], v[1], v[2]))
                result = bg;
        }
    }
    return result;
}

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;

    // === Масштабирование и центрирование под соотношение 3:2 ===
    float flag_aspect = 3.0 / 2.0;
    float screen_aspect = u_resolution.x / u_resolution.y;

    // Координаты в диапазоне [-1, 1]
    vec2 p = (uv - 0.5) * 2.0;
    if (screen_aspect > flag_aspect)
        p.x *= screen_aspect / flag_aspect;
    else
        p.y *= flag_aspect / screen_aspect;

    // Приведение к диапазону [0,1]
    p = p * 0.5 + 0.5;

    // === Базовые цвета ===
    vec4 red = vec4(0.933, 0.110, 0.145, 1.0);
    vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
    vec4 col = red;

    // === Система координат 30x20 (официальная сетка) ===
    vec2 flagPos = vec2(p.x * 30.0, p.y * 20.0);

    // === Большая звезда ===
    vec2 bigC = vec2(5.0, 20.0 - 5.0);
    float bigR = 3.0;           // внешний радиус
    float bigr = bigR * 0.382;  // внутренний радиус (золотое сечение)
    float bigAngle = PI / 2.0;

    col = Star(flagPos, bigC, bigr, bigR, bigAngle, yellow, col);

    // === Малые звёзды (точно по схеме) ===
    vec2 smallC[4];
    smallC[0] = vec2(10.0, 20.0 - 2.0);
    smallC[1] = vec2(12.0, 20.0 - 4.0);
    smallC[2] = vec2(12.0, 20.0 - 7.0);
    smallC[3] = vec2(10.0, 20.0 - 9.0);

    float smallR = 1.0;
    float smallr = smallR * 0.382;

    for (int i = 0; i < 4; i++) {
        vec2 c = smallC[i];
        vec2 dir = bigC - c;
        float angle = atan(dir.y, dir.x);
        col = Star(flagPos, c, smallr, smallR, angle, yellow, col);
    }

    // === Чёрный фон за пределами флага ===
    if (p.x < 0.0 || p.x > 1.0 || p.y < 0.0 || p.y > 1.0)
        col = vec4(0.0, 0.0, 0.0, 1.0);

    gl_FragColor = col;
}

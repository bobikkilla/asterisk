//Use Ctrl+Shift+P and write a command "Show glslCanvas" for live preview a fragment shaders (VSCodium)

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
#define PI 3.1415926

/*
    Функция, которая проверяет находятся ли точки pos и p0
    по одну сторону полуплоскости, получаемой в результате отсечения
    прямой, проходящей через точки v0 и v1.
*/
bool points_line_side(vec2 pos, vec2 p0, vec2 v0, vec2 v1) {
  return (pos.x - v0.x) * (v1.y - v0.y) < (pos.y - v0.y) * (v1.x - v0.x) &&
         (p0.x - v0.x) * (v1.y - v0.y) < (p0.y - v0.y) * (v1.x - v0.x);
}

/*
    Функция, которая проверяет находятся ли точка pos в области,
    которая получается с помощью двух пересекающихся линий.
*/
bool point_outside(vec2 pos, vec2 p0, vec2 v0, vec2 v1, vec2 v2) {
  if (!points_line_side(pos, p0, v0, v1) && !points_line_side(pos, p0, v1, v2))
    return true;
  return false;
}

/*
    Функция, которая рисует звезду
    pos - позиция точки, для которой задаем цвет;
    p0 - местоположение центра звезды;
    r - внутренний радиус (внутренняя граница звезды);
    R - внешний радиус (внешняя граница звезды);
    angle - угол поворота;
    color - цвет звезды;
    background_color - цвет фона.
*/
vec4 Star(vec2 pos, vec2 p0, float r, float R, float angle, vec4 color,
          vec4 background_color) {
  // Минимальное количество внешних вершин у звезды может быть задано 3
  const int count = 5;
  vec4 color_ = background_color;
  // Внешняя граница звезды определяется уравнением окружности
  if (pow(pos.x - p0.x, 2.0) + pow(pos.y - p0.y, 2.0) < pow(R, 2.0)) {
    // Устанавливаем цвет звезды
    color_ = color;
    for (int i = 0; i < count; ++i) {
      // 3 вершины образуют отсекаемую часть области, из них
      // две внешние и одна внутренняя
      vec2 vertices[3];
      for (int j = 0; j < 3; ++j) {
        // Итоговый порядковый номер вершины с учетом внутренних
        int index = i * 2 + j;
        // Угол, на который необходимо повернуть вершину
        // в зависимости от числа вершин, порядкового номера и общего угла
        float angle_ = angle + float(index) * PI / float(count);
        // Позиция смещения, относительно центра звезды
        vec2 shift =
            vec2(mod(float(index), 2.0) * r + mod(float(index + 1), 2.0) * R)
                .xy *
            vec2(cos(angle_), sin(angle_)).xy;
        // Позиция с учетом всего двумерного пространства
        vertices[j] = p0 + shift;
      }
      // Если точка принадлежит отсекаемой части области,
      // то отбрасываем цвет
      if (point_outside(pos, p0, vertices[0], vertices[1], vertices[2]))
        color_ = background_color;
    }
  }
  return color_;
}


// TODO: need to draw a mesh and show exactly that everything fits
void main() {
    // --- 1. Coordinate and Aspect Ratio Setup ---
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;

    // The flag has a 3:2 aspect ratio.
    // We adjust our coordinate system to match this, so our flag isn't stretched.
    float flag_aspect = 3.0 / 2.0;
    float screen_aspect = u_resolution.x / u_resolution.y;
    vec2 flag_uv = uv;
    if (screen_aspect > flag_aspect) {
        // Screen is wider than the flag (e.g., 16:9)
        // We shrink the x-coordinates to fit the 3:2 ratio
        flag_uv.x = (flag_uv.x - 0.5) * screen_aspect / flag_aspect + 0.5;
    } else {
        // Screen is taller than the flag (e.g., 9:16)
        // We shrink the y-coordinates to fit the 3:2 ratio
        flag_uv.y = (flag_uv.y - 0.5) * flag_aspect / screen_aspect + 0.5;
    }

    // Define colors
    vec4 flag_color = vec4(0.933, 0.110, 0.145, 1.0); // Red background for the flag
    vec4 star_color = vec4(1.0, 1.0, 0.0, 1.0); // Yellow stars

    // Start with the red background
    vec4 final_color = flag_color;

    // If the pixel is outside our corrected 0-1 range, use black for letterboxing
    if (flag_uv.x < 0.0 || flag_uv.x > 1.0 || flag_uv.y < 0.0 || flag_uv.y > 1.0) {
        final_color = vec4(0.0, 0.0, 0.0, 1.0);
        gl_FragColor = final_color;
        return;
    }

    // --- 2. Define Star Parameters based on the 30x20 grid ---
    // Schematic origin is top-left. GLSL origin is bottom-left.
    // Convert: x' = x/30, y' = (20-y)/20

    // Large Star: center (5,5), radius 3
    vec2 large_star_center = vec2(5.0/30.0, (20.0-5.0)/20.0);
    float large_star_R = 2.8 / 20.0; // Normalize radius by height
    float large_star_r = large_star_R * 0.4; // Inner radius approximation
    float large_star_angle = PI / 2.0; // Pointing upwards

    // Small Stars: radius 1
    // Coordinates from the schematic [ (10,2), (12,4), (12,7), (10,9) ]
    vec2 small_star_centers[4];
    small_star_centers[0] = vec2(10.0/30.0, (20.0-2.0)/20.0);
    small_star_centers[1] = vec2(12.0/30.0, (20.0-4.0)/20.0);
    small_star_centers[2] = vec2(12.0/30.0, (20.0-7.0)/20.0);
    small_star_centers[3] = vec2(10.0/30.0, (20.0-9.0)/20.0);

    float small_star_R = 1.0 / 20.0;
    float small_star_r = small_star_R * 0.4;


    // --- 3. Draw the Stars ---

    // Draw the large star first
    final_color = Star(flag_uv, large_star_center, large_star_r, large_star_R, large_star_angle, star_color, final_color);

    // Draw the four small stars, orienting each one towards the large star
    for (int i = 0; i < 4; i++) {
        vec2 current_center = small_star_centers[i];
        
        // Calculate the vector from the small star to the large star
        vec2 direction = large_star_center - current_center;
        
        // The angle for the star's point is the angle of that direction vector
        float angle = atan(direction.y, direction.x);
        
        // Pass the current composition as the "background" for the next star
        final_color = Star(flag_uv, current_center, small_star_r, small_star_R, angle, star_color, final_color);
    }

    // --- 4. Final Output ---
    gl_FragColor = final_color;
}
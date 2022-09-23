//
// Created by Alex on 9/10/2022.
//

#ifndef MINECRAFT_UTILS_H
#define MINECRAFT_UTILS_H

namespace minecraft::math {
    bool initialized = false;

    void init() {
        if (initialized) return;
        initialized = true;
        srand(time(NULL));
    }

    double random() {
        init();
        return (double) rand() / (RAND_MAX + 1.0);
    }

    int pmod(int a, int b) {
        int mod = a % (int) b;
        if (mod < 0) {
            mod += b;
        }
        return mod;
    }

    struct rect {
    private:
        glm::vec2 _pos = {0.0, 0.0};
        glm::vec2 _size = {0.0, 0.0};
    public:
        rect(float x, float y, float width, float height) {
            _pos.x = x;
            _pos.y = y;
            _size.x = width;
            _size.y = height;
        }

        glm::vec2 &pos() {
            return _pos;
        }

        glm::vec2 &size() {
            return _size;
        }

        float left() const {
            return x();
        }

        float right() const {
            return x() + width();
        }

        float top() const {
            return y();
        }

        float bottom() const {
            return y() + height();
        }

        float width() const {
            return _size.x;
        }

        float height() const {
            return _size.y;
        }

        float x() const {
            return _pos.x;
        }

        float y() const {
            return _pos.y;
        }

        float centerX() const {
            return _pos.x + width() / 2;
        }

        float centerY() const {
            return _pos.y + height() / 2;
        }

        rect &x(float v) {
            pos().x = v;
            return *this;
        }

        rect &y(float v) {
            pos().y = v;
            return *this;
        }

        rect &width(float v) {
            size().x = v;
            return *this;
        }

        rect &height(float v) {
            size().y = v;
            return *this;
        }

        rect &left(float v) {
            return x(v);
        }

        rect &right(float v) {
            return x(v - width());
        }

        rect &top(float v) {
            return y(v);
        }

        rect &bottom(float v) {
            return y(v - height());
        }

        rect &centerX(float v) {
            return x(v - width() / 2);
        }

        rect &centerY(float v) {
            return y(v - height() / 2);
        }

        rect &center(float x, float y) {
            return this->x(x).y(y);
        }

        rect &center(glm::vec2 pos) {
            return center(pos.x, pos.y);
        }

        std::string toString() {
            return std::string("Rect{x=") + std::to_string(x()) + ", y=" + std::to_string(y()) + ", w=" +
                   std::to_string(width()) + ", h=" + std::to_string(height()) + "}";
        }
    };
}

#endif //MINECRAFT_UTILS_H

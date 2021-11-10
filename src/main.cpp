#include <iostream>
#include <triangle.h>
#include <vector>
#include <random>

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_real_distribution<float> uniform(0.0f, 1.0f);

// Computes Integral[N . L, dA] / TriangleAreas using MonteCarlo
float computeShading(const std::vector<Triangle> &triangles, const vec3 &light, const int samples) {
    std::uniform_int_distribution<int> triangleDist(0, triangles.size() - 1);

    float integral = 0;
    const float trianglePickProbability = 1.0f / triangles.size();
    for (int i = 0; i < samples; i++) {
        // Pick a triangle
        const int triangleIndex = triangleDist(mt);
        const Triangle &triangle = triangles.at(triangleIndex);

        // Generate a random sample
        const vec3 point = triangle.uniformSample(uniform(mt), uniform(mt));
        const vec3 N = triangle.normal();
        const vec3 L = normalize(light - point);
        const float nDotL = dot(N, L);
        const float sampleProbability = trianglePickProbability * triangle.uniformSamplePdf();
        integral += nDotL / sampleProbability;
    }

    // Result of MC integration (Integral[N . L, dA])
    integral /= (float)samples;

    // We also divide by the total area (not related to MC)
    float totalArea = 0;
    for (const Triangle &triangle : triangles) {
        totalArea += triangle.area();
    }

    return integral / totalArea;
}

void run(const vec3 quad[2][4]) {
    const std::vector<Triangle> trapezoid = {
        Triangle(quad[0][0], quad[0][1], quad[0][2]),
        Triangle(quad[0][0], quad[0][2], quad[0][3])
    };

    std::cout << trapezoid[0] << std::endl;
    std::cout << trapezoid[1] << std::endl;

    const vec3 light(-50, -70, 20);
    int samples = 1;
    for (int i = 0; i < 7; i++) {
        std::cout << "computeShading(trapezoid, light, " << samples << "): " << computeShading(trapezoid, light, samples) << std::endl;
        samples *= 10;
    }
}

int main(int argc, char *argv[]) {
    const vec3 quadNotOk[2][4] = {
        { vec3(-30, -50, 0), vec3(-80, -50, 0), vec3(-80, -100, 0), vec3(70, -100, 0) },
        { vec3(70, -100, 40), vec3(-80, -100, 40), vec3(-80, -50, 40), vec3(-30, -50, 40) }
    };

    const vec3 quadOk[2][4] = {
        { vec3(-30, -50, 0), vec3(-80, -50, 0), vec3(-80, -100, 0), vec3(70, -100, 0) },
        { vec3(-30, -50, 40), vec3(-80, -50, 40), vec3(-80, -100, 40), vec3(70, -100, 40) }
    };

    std::cout << "quadNotOk" << std::endl;
    std::cout << "---------" << std::endl;
    run(quadNotOk);

    std::cout << std::endl;
    std::cout << "quadOk" << std::endl;
    std::cout << "------" << std::endl;
    run(quadOk);

    return EXIT_SUCCESS;
}
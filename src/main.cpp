#include <iostream>
#include <ctime>
#include <random>
#include <algorithm>
#include <cmath>
#include <vector>

// ===== ФУНКЦИЯ ГЕНЕРАТОРА (ВОЗВРАЩАЕТ ВЕКТОР) =====
std::vector<float> rundum(int n) {
    std::vector<float> r(n);
    static long b = time(nullptr); // Инициализация зерна

    for (int i = 0; i < n; i++) {
        b = (b * 1220703125L) % 2147483648L; // LCG: модуль 2^31
        if (b < 0) b += 2147483648L; // Корректировка для положительного значения
        r[i] = static_cast<float>(b) * 4.6566128730773926e-10f; // Нормализация до [0, 1)
    }
    return r;
}

// ===== ФУНКЦИЯ ТЕСТА КОЛМОГОРОВА-СМИРНОВА =====
float kolmogorovSmirnovTest(std::vector<float>& numbers) {
    int n = numbers.size();
    
    // Сортируем числа
    std::sort(numbers.begin(), numbers.end());
    
    // Вычисляем статистику D
    float D = 0.0f;
    for (int i = 0; i < n; i++) {
        float F_emp = (i + 1.0f) / n; // Эмпирическая функция распределения
        float F_theor = numbers[i];   // Теоретическая равномерная функция распределения
        D = std::max(D, std::abs(F_emp - F_theor));
    }
    
    return D;
}

// ===== ФУНКЦИЯ АВТОКОРРЕЛЯЦИОННОГО ТЕСТА =====
float autocorrelationTest(const std::vector<float>& numbers) {
    int n = numbers.size();
    float mean = 0.0f;
    
    // Вычисляем среднее
    for (int i = 0; i < n; i++) {
        mean += numbers[i];
    }
    mean /= n;
    
    // Вычисляем автокорреляцию с лагом 1
    float numerator = 0.0f;
    float denominator = 0.0f;
    for (int i = 0; i < n - 1; i++) {
        numerator += (numbers[i] - mean) * (numbers[i + 1] - mean);
        denominator += (numbers[i] - mean) * (numbers[i] - mean);
    }
    
    return numerator / denominator;
}

int main() {
    setlocale(0, "");
    int n = 10;

    // ===== ГЕНЕРАТОР ИЗ ФУНКЦИИ rundum =====
    std::vector<float> randomNumbers = rundum(n);

    std::cout << "Числа из функции rundum:" << std::endl;
    for (int i = 0; i < n; i++) {
        std::cout << randomNumbers[i] << std::endl;
    }

    // ===== ГЕНЕРАТОР ИЗ СТАНДАРТНОЙ БИБЛИОТЕКИ =====
    std::cout << "\nЧисла из стандартной библиотеки:" << std::endl;

    std::random_device rd; // Источник энтропии
    std::mt19937 gen(rd()); // Генератор Mersenne Twister
    std::uniform_real_distribution<float> dis(0.0f, 1.0f); // Равномерное распределение [0, 1)

    for (int i = 0; i < n; i++) {
        float random_value = dis(gen);
        std::cout << random_value << std::endl;
    }

    // ===== ТЕСТ КОЛМОГОРОВА-СМИРНОВА ДЛЯ rundum =====
    n = 1000;
    std::vector<float> testNumbers = rundum(n);

    float D = kolmogorovSmirnovTest(testNumbers);

    // Критическое значение для уровня значимости 0.05 и n=1000
    float critical_value = 1.36f / std::sqrt(n);

    std::cout << "\nТест Колмогорова-Смирнова для rundum:" << std::endl;
    std::cout << "Статистика D: " << D << std::endl;
    std::cout << "Критическое значение (0.05): " << critical_value << std::endl;
    if (D < critical_value) {
        std::cout << "Распределение равномерно" << std::endl;
    }
    else {
        std::cout << "Распределение не равномерно" << std::endl;
    }

    // ===== ТЕСТ КОЛМОГОРОВА-СМИРНОВА ДЛЯ СТАНДАРТНОГО ГЕНЕРАТОРА =====
    std::vector<float> stdNumbers;
    for (int i = 0; i < n; i++) {
        stdNumbers.push_back(dis(gen));
    }

    float D_std = kolmogorovSmirnovTest(stdNumbers);

    std::cout << "\nТест Колмогорова-Смирнова для стандартного генератора:" << std::endl;
    std::cout << "Статистика D: " << D_std << std::endl;
    std::cout << "Критическое значение (0.05): " << critical_value << std::endl;
    if (D_std < critical_value) {
        std::cout << "Распределение равномерно" << std::endl;
    }
    else {
        std::cout << "Распределение не равномерно" << std::endl;
    }

    // ===== ТЕСТЫ НЕЗАВИСИМОСТИ (АВТОКОРРЕЛЯЦИОННЫЙ ТЕСТ) =====
    std::cout << "\nТесты независимости (автокорреляционный тест):" << std::endl;

    // Тест для rundum
    std::vector<float> rundumNumbers = rundum(n);
    float autocorr_rundum = autocorrelationTest(rundumNumbers);

    std::cout << "\nАвтокорреляционный тест для rundum:" << std::endl;
    std::cout << "Коэффициент автокорреляции: " << autocorr_rundum << std::endl;
    if (std::abs(autocorr_rundum) < 0.1f) {
        std::cout << "Нет значимой автокорреляции" << std::endl;
    }
    else {
        std::cout << "Есть автокорреляция" << std::endl;
    }

    // Тест для стандартного генератора
    std::vector<float> stdNumbers2;
    for (int i = 0; i < n; i++) {
        stdNumbers2.push_back(dis(gen));
    }

    float autocorr_std = autocorrelationTest(stdNumbers2);

    std::cout << "\nАвтокорреляционный тест для стандартного генератора:" << std::endl;
    std::cout << "Коэффициент автокорреляции: " << autocorr_std << std::endl;
    if (std::abs(autocorr_std) < 0.1f) {
        std::cout << "Нет значимой автокорреляции" << std::endl;
    }
    else {
        std::cout << "Есть автокорреляция" << std::endl;
    }

    return 0;
}
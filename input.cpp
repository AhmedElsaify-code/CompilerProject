int main() {
    int age = 20;
    float grade = 95.5;
    char letter = 'A';
    bool passed = true;

    if (age >= 18 && passed) {
        age = age + 1;
    } else {
        age = 0;
    }

    while (age < 25) {
        age = age + 1;
    }

    return 0;
}
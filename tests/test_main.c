#include <stdio.h>

int test_common(void);
int test_circuit(void);
int test_graph(void);
int test_input(void);
int test_analysis(void);
int test_sim(void);
int test_qml(void);

int main(void)
{
    int failures = 0;

    failures += test_common();
    failures += test_circuit();
    failures += test_graph();
    failures += test_input();
    failures += test_analysis();
    failures += test_sim();
    failures += test_qml();

    if (failures == 0) {
        printf("All foundation tests passed.\n");
        return 0;
    }

    printf("Foundation tests failed: %d\n", failures);
    return 1;
}

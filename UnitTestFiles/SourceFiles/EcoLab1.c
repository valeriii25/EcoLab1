/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int __cdecl comp_int_std(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int16_t ECOCALLMETHOD compare_int(const void* a, const void* b) {
    if (*(int*)a > *(int*)b) return 1;
    if (*(int*)a < *(int*)b) return -1;
    return 0;
}

int16_t ECOCALLMETHOD compare_float(const void* a, const void* b) {
    if (*(float*)a > *(float*)b) return 1;
    if (*(float*)a < *(float*)b) return -1;
    return 0;
}

int16_t ECOCALLMETHOD compare_double(const void* a, const void* b) {
    if (*(double*)a > *(double*)b) return 1;
    if (*(double*)a < *(double*)b) return -1;
    return 0;
}

int16_t ECOCALLMETHOD compare_long_double(const void* a, const void* b) {
    if (*(long double*)a > *(long double*)b) return 1;
    if (*(long double*)a < *(long double*)b) return -1;
    return 0;
}

void run_performance_tests(IEcoLab1* pIEcoLab1, IEcoMemoryAllocator1* pIMem) {
    int sizes[] = {1000, 5000, 10000, 20000};
    int n_sizes = sizeof(sizes)/sizeof(sizes[0]);
    int i, j;

    srand(time(0));

    printf("Performance Comparison\n");

    for (i = 0; i < n_sizes; i++) {
		clock_t start_time, end_time;
        double cpu_time_used;
        int size = sizes[i];
        int* arr1 = (int*)pIMem->pVTbl->Alloc(pIMem, size * sizeof(int));
        int* arr2 = (int*)pIMem->pVTbl->Alloc(pIMem, size * sizeof(int));

        if (arr1 == 0 || arr2 == 0) {
            printf("Memory allocation failed for size %d\n", size);
            if(arr1) pIMem->pVTbl->Free(pIMem, arr1);
            if(arr2) pIMem->pVTbl->Free(pIMem, arr2);
            continue;
        }

        for (j = 0; j < size; j++) {
            arr1[j] = rand() % 20001 - 10000;
            arr2[j] = arr1[j];
        }

        start_time = clock();
        pIEcoLab1->pVTbl->SelectionSort(pIEcoLab1, arr1, size, sizeof(int), compare_int);
        end_time = clock();
        cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000.0;
        printf("Array size: %d\n", size);
        printf("  My SelectionSort time: %.2f ms\n", cpu_time_used);

        start_time = clock();
        qsort(arr2, size, sizeof(int), comp_int_std);
        end_time = clock();
        cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000.0;
        printf("  Standard qsort time:   %.2f ms\n", cpu_time_used);
        printf("\n");

        pIMem->pVTbl->Free(pIMem, arr1);
        pIMem->pVTbl->Free(pIMem, arr2);
    }
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    /* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;
    int i;

	int arr_int[] = {64, -34, 25, -12, 22, 11, 90};
    int n_int = sizeof(arr_int)/sizeof(arr_int[0]);

	float arr_float[] = {90.7, 80.6, 60.5, 50.4, 40.3, 30.2, 20.1};
    int n_float = sizeof(arr_float)/sizeof(arr_float[0]);

	double arr_double[] = {11.1, 12.2, 22.3, 25.4, 34.5, 64.6, 90.7};
    int n_double = sizeof(arr_double)/sizeof(arr_double[0]);

	long double arr_ld[] = {-10.1, 20.2, -30.3, 20.2, -10.1, 0.0, 40.4};
    int n_ld = sizeof(arr_ld)/sizeof(arr_ld[0]);

    /* Проверка и создание системного интрефейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
			/* Освобождение системного интерфейса в случае ошибки */
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Освобождение в случае ошибки */
		goto Release;
    }
#ifdef ECO_LIB
	/* Регистрация статического компонента для работы со списком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
		/* Освобождение в случае ошибки */
        goto Release;
    }
#endif
	/* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    /* Проверка */
	if (result != 0 || pIMem == 0) {
		/* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

	/* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
		/* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

    printf("Test 1: Integer array\n");
    printf("Initial array: ");
    for (i=0; i < n_int; i++) printf("%d ", arr_int[i]);
    printf("\n");
    pIEcoLab1->pVTbl->SelectionSort(pIEcoLab1, arr_int, n_int, sizeof(int), compare_int);
    printf("Sorted array:   ");
    for (i=0; i < n_int; i++) printf("%d ", arr_int[i]);
    printf("\n\n");

    printf("Test 2: Float array (reverse sorted)\n");
    printf("Initial array: ");
    for (i=0; i < n_float; i++) printf("%.2f ", arr_float[i]);
    printf("\n");
    pIEcoLab1->pVTbl->SelectionSort(pIEcoLab1, arr_float, n_float, sizeof(float), compare_float);
    printf("Sorted array:   ");
    for (i=0; i < n_float; i++) printf("%.2f ", arr_float[i]);
    printf("\n\n");

    printf("Test 3: Double array (already sorted)\n");
    printf("Initial array: ");
    for (i=0; i < n_double; i++) printf("%.2lf ", arr_double[i]);
    printf("\n");
    pIEcoLab1->pVTbl->SelectionSort(pIEcoLab1, arr_double, n_double, sizeof(double), compare_double);
    printf("Sorted array:   ");
    for (i=0; i < n_double; i++) printf("%.2lf ", arr_double[i]);
    printf("\n\n");

    printf("Test 4: Long Double array (with duplicates)\n");
    printf("Initial array: ");
    for (i=0; i < n_ld; i++) printf("%.2Lf ", arr_ld[i]);
    printf("\n");
    pIEcoLab1->pVTbl->SelectionSort(pIEcoLab1, arr_ld, n_ld, sizeof(long double), compare_long_double);
    printf("Sorted array:   ");
    for (i=0; i < n_ld; i++) printf("%.2Lf ", arr_ld[i]);
    printf("\n\n");

    run_performance_tests(pIEcoLab1, pIMem);

	printf("Enter any symbol and press \"ENTER\" to end");
	getchar();

Release:

	/* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

	/* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

	/* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) {
        pIEcoLab1->pVTbl->Release(pIEcoLab1);
    }
	
    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}
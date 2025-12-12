#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_MODULES 100
#define MAX_BUGS 100
#define MAX_NAME_LEN 100
int findModuleIndex(char *name);
void dfs(int v);
void suggestFixOrder();
void traceBugOrigin(char *moduleName);
void findFixBacktrackOrder(char *moduleName);
void exportReportToFile();
void clearBuffer();
void removeNewline(char* str);
void addModule();
void addDependency();
void showAllModules();
void showDependencyGraph();
void reportBug();
void showBugs();
void fixBug();
void showMenu();
typedef struct Bug {
    int bugID;
    char description[200];
    char type[50];
    char priority[10];
    int isFixed;
    char fixHistory[200];
} Bug;
typedef struct Module {
    int id;
    char name[MAX_NAME_LEN];
    Bug bugs[MAX_BUGS];
    int bugCount;
} Module;
Module modules[MAX_MODULES];
int moduleCount = 0;
int bugIDCounter = 1;
int adjList[MAX_MODULES][MAX_MODULES];
int adjMatrix[MAX_MODULES][MAX_MODULES];
int visited[MAX_MODULES], stack[MAX_MODULES], top = -1;
void dfs(int v) {
    visited[v] = 1;
    for (int i = 0; i < moduleCount; i++) {
        if (adjMatrix[v][i] && !visited[i]) {
            dfs(i);
        }
    }
    stack[++top] = v;
}
void suggestFixOrder() {
    memset(visited, 0, sizeof(visited));
    top = -1;

    for (int i = 0; i < moduleCount; i++) {
        if (!visited[i]) {
            dfs(i);
        }
    }
    printf("\n🛠️ Suggested Fix Order (respecting dependencies):\n");
    for (int i = 0; i <=top; i++) {
        printf("  ➤ %s\n", modules[stack[i]].name);
    }
}
void traceBugOrigin(char *moduleName) {
    int moduleIndex = findModuleIndex(moduleName);
    if (moduleIndex == -1) {
        printf("Module '%s' not found.\n", moduleName);
        return;
    }
    printf("\n🔍 Tracing Bug Origin for Module %s\n", moduleName);
    int potentialOrigins[MAX_MODULES];
    int originCount = 0;
    int visitedTrace[MAX_MODULES] = {0};
    int queue[MAX_MODULES];
    int front = 0, rear = 0;
    queue[rear++] = moduleIndex;
    visitedTrace[moduleIndex] = 1;
    while (front < rear) {
        int currentModuleIndex = queue[front++];
        int dependsOnCount = 0;
        for (int i = 0; i < moduleCount; i++) {
            if (adjMatrix[currentModuleIndex][i]) {
                if (dependsOnCount == 0) 
                dependsOnCount++;
                if (!visitedTrace[i]) {
                    visitedTrace[i] = 1;
                    queue[rear++] = i;
                }
            }
        }
        for (int i = 0; i < modules[currentModuleIndex].bugCount; i++) {
            if (!modules[currentModuleIndex].bugs[i].isFixed) {
                int alreadyFound = 0;
                for (int k = 0; k < originCount; k++) {
                    if (potentialOrigins[k] == currentModuleIndex) {
                        alreadyFound = 1;
                        break;
                    }
                }
                if (!alreadyFound) {
                    potentialOrigins[originCount++] = currentModuleIndex;
                }
            }
        }
    }
    printf("\n✅ Potential bug origins in the dependency chain:\n");
    if (originCount > 0) {
        for (int i = originCount-1; i >= 0; i--) {
            printf("  - %s (Unresolved Bugs Present)\n", modules[potentialOrigins[i]].name);
        }
    } else {
        printf("  No unresolved bugs found in the dependency chain.\n");
    }
}
void exportReportToFile() {
    FILE *fp = fopen("bug_report.txt", "w");
    if (!fp) {
        printf("❌ Error creating report file.\n");
        return;
    }
    fprintf(fp, "=== 🐞 Bug Tracking Report ===\n\n");
    for (int i = 0; i < moduleCount; i++) {
        fprintf(fp, "Module: %s\n", modules[i].name);
        for (int j = 0; j < modules[i].bugCount; j++) {
            Bug *bug = &modules[i].bugs[j];
            fprintf(fp, "  [ID %d] %s | Type: %s | Priority: %s | Status: %s | Fix History: %s\n",
                    bug->bugID, bug->description, bug->type,
                    bug->priority, bug->isFixed ? "Fixed" : "Pending", bug->fixHistory);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n=== 🛠️ Suggested Fix Order ===\n");
    memset(visited, 0, sizeof(visited));
    top = -1;
    for (int i = 0; i < moduleCount; i++) {
        if (!visited[i]) {
            dfs(i);
        }
    }
    for (int i = 0; i <= top; i++) {
        fprintf(fp, "  ➤ %s\n", modules[stack[i]].name);
    }
    fclose(fp);
    printf("✅ Bug report exported to 'bug_report.txt'.\n");
}
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
void removeNewline(char* str) {
    str[strcspn(str, "\n")] = 0;
}
int findModuleIndex(char *name) {
    for (int i = 0; i < moduleCount; i++) {
        if (strcmp(modules[i].name, name) == 0)
            return i;
    }
    return -1;
}
void addModule() {
    if (moduleCount >= MAX_MODULES) {
        printf("Maximum module limit reached.\n");
        return;
    }
    char name[MAX_NAME_LEN];
    printf("Enter module name: ");
    fgets(name, MAX_NAME_LEN, stdin);
    removeNewline(name);
    if (findModuleIndex(name) != -1) {
        printf("Module already exists.\n");
        return;
    }
    modules[moduleCount].id = moduleCount;
    strcpy(modules[moduleCount].name, name);
    modules[moduleCount].bugCount = 0;
    moduleCount++;
    printf("Module '%s' added successfully.\n", name);
}
void addDependency() {
    char from[MAX_NAME_LEN], to[MAX_NAME_LEN];
    printf("Enter dependent module: ");
    fgets(from, MAX_NAME_LEN, stdin);
    removeNewline(from);
    printf("Depends on module: ");
    fgets(to, MAX_NAME_LEN, stdin);
    removeNewline(to);
    int fromID = findModuleIndex(from);
    int toID = findModuleIndex(to);
    if (fromID == -1 || toID == -1) {
        printf("Invalid module names.\n");
        return;
    }
    adjList[fromID][toID] = 1;
    adjMatrix[fromID][toID] = 1;
    printf("Dependency added: %s ➡️ %s\n", from, to);
}
void showAllModules() {
    if (moduleCount == 0) {
        printf("No modules added yet.\n");
        return;
    }
    printf("\n📋 List of Modules:\n");
    for (int i = 0; i < moduleCount; i++) {
        printf("  [%d] %s\n", modules[i].id, modules[i].name);
    }
}
void showDependencyGraph() {
    printf("\n📈 Dependency Graph:\n");
    for (int i = 0; i < moduleCount; i++) {
        printf("Module '%s' depends on: ", modules[i].name);
        int hasDep = 0;
        for (int j = 0; j < moduleCount; j++) {
            if (adjList[i][j]) {
                printf("%s ", modules[j].name);
                hasDep = 1;
            }
        }
        if (!hasDep) printf("None");
        printf("\n");
    }
}
void reportBug() {
    char moduleName[MAX_NAME_LEN];
    printf("Enter module name to report bug: ");
    fgets(moduleName, MAX_NAME_LEN, stdin);
    removeNewline(moduleName);
    int idx = findModuleIndex(moduleName);
    if (idx == -1) {
        printf("Module not found.\n");
        return;
    }
    if (modules[idx].bugCount >= MAX_BUGS) {
        printf("Bug limit reached for module.\n");
        return;
    }
    Bug *bug = &modules[idx].bugs[modules[idx].bugCount];
    bug->bugID = bugIDCounter++;
    printf("Enter bug description: ");
    fgets(bug->description, sizeof(bug->description), stdin);
    removeNewline(bug->description);
    printf("Enter bug type (UI/Logic/Performance/etc.): ");
    fgets(bug->type, sizeof(bug->type), stdin);
    removeNewline(bug->type);
    printf("Enter priority (High/Medium/Low): ");
    fgets(bug->priority, sizeof(bug->priority), stdin);
    removeNewline(bug->priority);
    bug->isFixed = 0;
    strcpy(bug->fixHistory, "Not fixed yet.");
    modules[idx].bugCount++;
    printf("Bug reported to '%s'\n", moduleName);
}
void showBugs() {
    printf("\n📊 Bug Status Report:\n");
    for (int i = 0; i < moduleCount; i++) {
        if (modules[i].bugCount == 0) continue;
        printf("\nModule: %s\n", modules[i].name);
        for (int j = 0; j < modules[i].bugCount; j++) {
            Bug *bug = &modules[i].bugs[j];
            printf("  [ID %d] %s | Type: %s | Priority: %s | Status: %s | Fix History: %s\n",
                    bug->bugID, bug->description, bug->type,
                    bug->priority, bug->isFixed ? "✅ Fixed" : "❌ Pending", bug->fixHistory);
        }
    }
}void findUnresolvedDependencies(int idx, int visited[], int unresolved[], int *count) {
    visited[idx] = 1;
    for (int i = 0; i < moduleCount; i++) {
        if (adjMatrix[idx][i] == 1 && !visited[i]) {
            for (int j = 0; j < modules[i].bugCount; j++) {
                if (!modules[i].bugs[j].isFixed) {
                    unresolved[*count] = i;
                    (*count)++;
                    break;
                }
            }
            findUnresolvedDependencies(i, visited, unresolved, count);
        }
    }
}
void fixBug() {
    char module[MAX_NAME_LEN];
    printf("Enter the module name to fix its bugs: ");
    fgets(module, MAX_NAME_LEN, stdin);
    removeNewline(module);
    int idx = findModuleIndex(module);
    if (idx == -1) {
        printf("Module not found!\n");
        return;
    }
    int visited[MAX_MODULES] = {0};
    int unresolved[MAX_MODULES];
    int count = 0;
    findUnresolvedDependencies(idx, visited, unresolved, &count);
    if (count > 0) {
        printf("❌ Cannot fix bugs in '%s' because the following dependent modules have unresolved bugs:\n", module);
        for (int i = count-1; i >=0; i--) {
            printf("   ➤ %s\n", modules[unresolved[i]].name);
        }
        return;
    }
    int fixedAny = 0;
    for (int i = 0; i < modules[idx].bugCount; i++) {
        if (!modules[idx].bugs[i].isFixed) {
            modules[idx].bugs[i].isFixed = 1;
            strcpy(modules[idx].bugs[i].fixHistory, "Fixed on latest patch.");
            printf("✅ Fixed bug ID %d: %s\n", modules[idx].bugs[i].bugID, modules[idx].bugs[i].description);
            fixedAny = 1;
        }
    }
    if (!fixedAny) {
        printf("✅ No pending bugs in module '%s'.\n", module);
    }
}
void showMenu() {
    printf("\n===== Bug Tracker Menu =====\n");
    printf("1. Add Module\n");
    printf("2. Add Dependency\n");
    printf("3. Report Bug\n");
    printf("4. Fix Bugs in Module\n");
    printf("5. Show Modules\n");
    printf("6. Show Dependency Graph\n");
    printf("7. Show Bug Report\n");
    printf("8. Suggest Fix Order\n");
    printf("9. Trace Bug Origin\n");
    printf("10. Export Bug Report to File\n");
    printf("0. Exit\n");
    printf("Choose an option: ");
}
int main() {
    int choice;
    while (1) {
        showMenu();
        scanf("%d", &choice);
        clearBuffer();   
        switch (choice) {
            case 1: addModule(); break;
            case 2: addDependency(); break;
            case 3: reportBug(); break;
            case 4: fixBug(); break;
            case 5: showAllModules(); break;
            case 6: showDependencyGraph(); break;
            case 7: showBugs(); break;
            case 8: suggestFixOrder(); break;
            case 9: {
                char moduleName[MAX_NAME_LEN];
                printf("Enter module name to trace bug origin: ");
                fgets(moduleName, MAX_NAME_LEN, stdin);
                removeNewline(moduleName);
                traceBugOrigin(moduleName);
                break;
            }
            case 10: exportReportToFile(); break;
            case 0: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}
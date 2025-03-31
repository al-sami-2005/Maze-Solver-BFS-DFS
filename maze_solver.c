#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define ROW 5
#define COL 5

// ANSI Color Codes
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"

// Directions for moving up, down, left, and right
int dRow[] = {-1, 1, 0, 0};
int dCol[] = {0, 0, -1, 1};

// Original Maze Representation (for reset)
const char originalMaze[ROW][COL] = {
    {'S', '0', '1', '0', 'E'},
    {'0', '1', '0', '1', '0'},
    {'0', '0', 'S', '1', '0'},
    {'1', '1', '0', '0', 'E'},
    {'E', '0', '1', '1', '0'}
};

// Current Maze Representation
char maze[ROW][COL];

int stepMap[ROW][COL]; // To track steps taken to reach each cell

// Structure to hold queue and stack elements
typedef struct {
    int x, y;
    int steps;
} Node;

// Function to reset maze to original state
void resetMaze() {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            maze[i][j] = originalMaze[i][j];
        }
    }
    printf(GREEN "\nMaze reset to original state!\n" RESET);
}

// Function to update obstacles dynamically
void updateObstacles() {
    srand(time(0));
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (maze[i][j] == '0' && (rand() % 10) < 2) { // 20% chance to turn into an obstacle
                maze[i][j] = '1';
            }
        }
    }
    printf(GREEN "Obstacles updated!\n" RESET);
}

// Function to print the maze with colors
void printColoredMaze() {
    printf("\nCurrent Maze:\n");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            // Apply colors based on cell type
            if (maze[i][j] == 'S') {
                printf(GREEN " %c " RESET, maze[i][j]); // Start = green
            } else if (maze[i][j] == 'E') {
                printf(RED " %c " RESET, maze[i][j]);   // End = red
            } else if (maze[i][j] == '1') {
                printf(YELLOW " %c " RESET, maze[i][j]); // Wall = yellow
            } else if (stepMap[i][j] != -1) {
                printf(BLUE " * " RESET);               // Visited path = blue
            } else {
                printf(WHITE " %c " RESET, maze[i][j]); // Empty = white
            }
        }
        printf("\n");
    }
}

// Function to input maze from user
void inputMaze() {
    printf("\nEnter your %dx%d maze (one row at a time):\n", ROW, COL);
    printf("S = Start, E = End, 0 = Path, 1 = Wall\n");
    
    for (int i = 0; i < ROW; i++) {
        printf("Row %d (separate with spaces): ", i+1);
        for (int j = 0; j < COL; j++) {
            scanf(" %c", &maze[i][j]);
            // Validate input
            while(maze[i][j] != 'S' && maze[i][j] != 'E' && 
                  maze[i][j] != '0' && maze[i][j] != '1') {
                printf("Invalid input at [%d][%d]. Enter S/E/0/1: ", i, j);
                scanf(" %c", &maze[i][j]);
            }
        }
    }
    printf(GREEN "\nMaze successfully entered!\n" RESET);
}

// Function to read maze from file
void readMazeFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf(RED "\nError: Could not open file '%s'\n" RESET, filename);
        return;
    }
    
    printf("\nReading maze from file...\n");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (fscanf(file, " %c", &maze[i][j]) != 1) {
                printf(RED "Error reading file at row %d, column %d\n" RESET, i+1, j+1);
                fclose(file);
                return;
            }
            // Validate input from file
            if (maze[i][j] != 'S' && maze[i][j] != 'E' && 
                maze[i][j] != '0' && maze[i][j] != '1') {
                printf(RED "Invalid character '%c' at row %d, column %d\n" RESET, maze[i][j], i+1, j+1);
                fclose(file);
                return;
            }
        }
    }
    fclose(file);
    printf(GREEN "Maze successfully loaded from file!\n" RESET);
}

// BFS function
void solveMazeBFS() {
    clock_t start = clock();
    printf(CYAN "\nSolving Maze using BFS..." RESET);
    
    bool visited[ROW][COL] = {false};
    Node queue[ROW * COL];
    int front = 0, rear = 0;
    
    // Initialize stepMap
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            stepMap[i][j] = -1;
        }
    }
    
    // Add all start points to the queue
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (maze[i][j] == 'S') {
                queue[rear++] = (Node){i, j, 0};
                visited[i][j] = true;
                stepMap[i][j] = 0;
            }
        }
    }
    
    while (front < rear) {
        Node current = queue[front++];
        
        if (maze[current.x][current.y] == 'E') {
            printf(GREEN "\nMaze solved in %d steps using BFS!" RESET, current.steps);
            clock_t end = clock();
            printf(MAGENTA "\nBFS Execution Time: %lf seconds\n" RESET, (double)(end - start) / CLOCKS_PER_SEC);
            printColoredMaze();
            return;
        }
        
        for (int i = 0; i < 4; i++) {
            int newX = current.x + dRow[i];
            int newY = current.y + dCol[i];
            
            if (newX >= 0 && newX < ROW && newY >= 0 && newY < COL &&
                maze[newX][newY] != '1' && !visited[newX][newY]) {
                queue[rear++] = (Node){newX, newY, current.steps + 1};
                visited[newX][newY] = true;
                stepMap[newX][newY] = current.steps + 1;
            }
        }
    }
    
    printf(RED "\nNo path found using BFS.\n" RESET);
}

// DFS function
void solveMazeDFS() {
    clock_t start = clock();
    printf(CYAN "\nSolving Maze using DFS..." RESET);
    
    bool visited[ROW][COL] = {false};
    Node stack[ROW * COL];
    int top = -1;
    
    // Initialize stepMap
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            stepMap[i][j] = -1;
        }
    }
    
    // Push all start points to the stack
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (maze[i][j] == 'S') {
                stack[++top] = (Node){i, j, 0};
                visited[i][j] = true;
                stepMap[i][j] = 0;
            }
        }
    }
    
    while (top >= 0) {
        Node current = stack[top--];
        
        if (maze[current.x][current.y] == 'E') {
            printf(GREEN "\nMaze solved in %d steps using DFS!" RESET, current.steps);
            clock_t end = clock();
            printf(MAGENTA "\nDFS Execution Time: %lf seconds\n" RESET, (double)(end - start) / CLOCKS_PER_SEC);
            printColoredMaze();
            return;
        }
        
        for (int i = 0; i < 4; i++) {
            int newX = current.x + dRow[i];
            int newY = current.y + dCol[i];
            
            if (newX >= 0 && newX < ROW && newY >= 0 && newY < COL &&
                maze[newX][newY] != '1' && !visited[newX][newY]) {
                stack[++top] = (Node){newX, newY, current.steps + 1};
                visited[newX][newY] = true;
                stepMap[newX][newY] = current.steps + 1;
            }
        }
    }
    
    printf(RED "\nNo path found using DFS.\n" RESET);
}

int main() {
    // Initialize maze with original state
    resetMaze();
    
    int choice;
    char filename[100];
    
    while(1) {
        printf(BLUE "\nMaze BFS & DFS Solver\n" RESET);
        printf("=======================\n");
        
        printf("\n1. Use default maze\n");
        printf("2. Enter maze manually\n");
        printf("3. Load maze from file\n");
        printf("4. Reset to original maze\n");
        printf("5. Exit\n");
        printf("Choose option (1-5): ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                printf(GREEN "\nUsing default maze configuration\n" RESET);
                resetMaze();
                break;
            case 2:
                inputMaze();
                break;
            case 3:
                printf("\nEnter filename (e.g., maze.txt): ");
                scanf("%99s", filename);
                readMazeFromFile(filename);
                break;
            case 4:
                resetMaze();
                break;
            case 5:
                printf("\nExiting program...\n");
                return 0;
            default:
                printf(RED "\nInvalid choice, please try again\n" RESET);
                continue;
        }
        
        // Display current maze
        printf("\nCurrent Maze:");
        printColoredMaze();
        
        // Update obstacles and solve
        updateObstacles();
        solveMazeBFS();
        
        // Reset stepMap for DFS
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                stepMap[i][j] = -1;
            }
        }
        
        solveMazeDFS();
    }
    
    return 0;
}
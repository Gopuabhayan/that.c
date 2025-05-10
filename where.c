#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// For cross-platform compatibility with keyboard input
#ifdef _WIN32
    #include <conio.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define CLEAR_SCREEN "clear"
    
    // Implementation of kbhit() and getch() for Unix-like systems
    int kbhit(void) {
        struct termios oldt, newt;
        int ch, oldf;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        
        if(ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }
        
        return 0;
    }
    
    int getch(void) {
        struct termios oldt, newt;
        int ch;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        
        return ch;
    }
#endif

// Game constants
#define WIDTH 30
#define HEIGHT 20
#define INITIAL_SNAKE_LENGTH 3

// Directions
enum Direction {
    UP, 
    DOWN, 
    LEFT, 
    RIGHT
};

// Structure to represent a segment of the snake
typedef struct {
    int x;
    int y;
} Segment;

// Game state
typedef struct {
    Segment snake[WIDTH * HEIGHT]; // Maximum possible length
    int length;
    enum Direction direction;
    Segment food;
    int score;
    bool gameOver;
} Game;

// Function prototypes
void initializeGame(Game *game);
void generateFood(Game *game);
void drawGame(Game *game);
void updateGame(Game *game);
void handleInput(Game *game);
bool isCollision(Game *game, int x, int y);
void delay(int milliseconds);

int main() {
    Game game;
    
    // Seed random number generator
    srand(time(NULL));
    
    // Initialize game state
    initializeGame(&game);
    
    // Main game loop
    while (!game.gameOver) {
        // Handle user input
        handleInput(&game);
        
        // Update game state
        updateGame(&game);
        
        // Draw the game
        drawGame(&game);
        
        // Add a small delay to control game speed
        delay(150);
    }
    
    // Display final score
    printf("\nGame Over! Final Score: %d\n", game.score);
    
    return 0;
}

// Initialize the game state
void initializeGame(Game *game) {
    // Set initial snake position (middle of the screen)
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    
    // Create initial snake segments
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
        game->snake[i].x = startX - i;
        game->snake[i].y = startY;
    }
    
    game->length = INITIAL_SNAKE_LENGTH;
    game->direction = RIGHT;
    game->score = 0;
    game->gameOver = false;
    
    // Generate initial food
    generateFood(game);
}

// Generate food at a random position
void generateFood(Game *game) {
    bool validPosition = false;
    
    // Keep generating positions until we find one that's not on the snake
    while (!validPosition) {
        game->food.x = rand() % WIDTH;
        game->food.y = rand() % HEIGHT;
        
        validPosition = true;
        
        // Check if food is on the snake
        for (int i = 0; i < game->length; i++) {
            if (game->snake[i].x == game->food.x && game->snake[i].y == game->food.y) {
                validPosition = false;
                break;
            }
        }
    }
}

// Draw the current game state
void drawGame(Game *game) {
    // Clear the screen
    system(CLEAR_SCREEN);
    
    // Create a buffer to represent the game field
    char buffer[HEIGHT][WIDTH];
    
    // Initialize buffer with empty spaces
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer[y][x] = ' ';
        }
    }
    
    // Draw snake
    for (int i = 0; i < game->length; i++) {
        if (game->snake[i].x >= 0 && game->snake[i].x < WIDTH && 
            game->snake[i].y >= 0 && game->snake[i].y < HEIGHT) {
            if (i == 0) {
                // Snake head
                buffer[game->snake[i].y][game->snake[i].x] = 'O';
            } else {
                // Snake body
                buffer[game->snake[i].y][game->snake[i].x] = 'o';
            }
        }
    }
    
    // Draw food
    buffer[game->food.y][game->food.x] = '*';
    
    // Print the buffer with borders
    printf("┌");
    for (int x = 0; x < WIDTH; x++) {
        printf("─");
    }
    printf("┐\n");
    
    for (int y = 0; y < HEIGHT; y++) {
        printf("│");
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", buffer[y][x]);
        }
        printf("│\n");
    }
    
    printf("└");
    for (int x = 0; x < WIDTH; x++) {
        printf("─");
    }
    printf("┘\n");
    
    // Display score
    printf("Score: %d\n", game->score);
    printf("Controls: W (up), S (down), A (left), D (right), Q (quit)\n");
}

// Update game state
void updateGame(Game *game) {
    // Calculate new head position
    int newHeadX = game->snake[0].x;
    int newHeadY = game->snake[0].y;
    
    // Update position based on direction
    switch (game->direction) {
        case UP:
            newHeadY--;
            break;
        case DOWN:
            newHeadY++;
            break;
        case LEFT:
            newHeadX--;
            break;
        case RIGHT:
            newHeadX++;
            break;
    }
    
    // Check for collision with walls
    if (newHeadX < 0 || newHeadX >= WIDTH || newHeadY < 0 || newHeadY >= HEIGHT) {
        game->gameOver = true;
        return;
    }
    
    // Check for collision with self
    if (isCollision(game, newHeadX, newHeadY)) {
        game->gameOver = true;
        return;
    }
    
    // Check if snake eats food
    bool ateFood = (newHeadX == game->food.x && newHeadY == game->food.y);
    
    // If snake ate food, increase length
    if (ateFood) {
        game->length++;
        game->score += 10;
        generateFood(game);
    }
    
    // Move snake body (from tail to head)
    for (int i = game->length - 1; i > 0; i--) {
        game->snake[i] = game->snake[i - 1];
    }
    
    // Update head position
    game->snake[0].x = newHeadX;
    game->snake[0].y = newHeadY;
}

// Handle user input
void handleInput(Game *game) {
    if (kbhit()) {
        char key = getch();
        
        switch (key) {
            case 'w': case 'W':
                if (game->direction != DOWN) {
                    game->direction = UP;
                }
                break;
            case 's': case 'S':
                if (game->direction != UP) {
                    game->direction = DOWN;
                }
                break;
            case 'a': case 'A':
                if (game->direction != RIGHT) {
                    game->direction = LEFT;
                }
                break;
            case 'd': case 'D':
                if (game->direction != LEFT) {
                    game->direction = RIGHT;
                }
                break;
            case 'q': case 'Q':
                game->gameOver = true;
                break;
        }
    }
}

// Check if there's a collision with the snake body
bool isCollision(Game *game, int x, int y) {
    for (int i = 0; i < game->length; i++) {
        if (game->snake[i].x == x && game->snake[i].y == y) {
            return true;
        }
    }
    return false;
}

// Delay function to control game speed
void delay(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #else
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #endif
}
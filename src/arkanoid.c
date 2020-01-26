/*
 * arkanoid.c
 *
 *  Created on: Feb 24, 2017
 *  Edited  on: Nov 15, 2019
 *      Author: hathweels
 */

#include "inc/arkanoid.h"

extern FILE *hKeyboard;

static const long difficulty[6] = { 100, 90, 80, 70, 60, 50 };
static size_t user_diff = 2;

static const char *game_end[] = { "RESTART", "QUIT" };
static const char *game_pause[] = { "CONTINUE", "DIFFICULTY", "RESTART", "QUIT" };
static const char *game_difficulty[] = { "VERY EASY", "EASY", "NORMAL", "HARD", "VERY HARD", "INSANE" };


static long arkanoid_get_difficulty (void) {
  return difficulty[user_diff];
}

static void arkanoid_set_difficulty (size_t new_diff) {
  user_diff = new_diff;
}

static int arkanoid_bounce (blob* object, blob* ball) {
  int i, j;

  for (i = object->xs; i <= object->xe; i++) {
      for (j = object->ys; j <= object->ye; j++) {
        if ((ball->xs == i && ball->ys == j) ||
            (ball->xs == i && ball->ye == j) ||
            (ball->xe == i && ball->ys == j) ||
            (ball->xe == i && ball->ye == j))
          return 1; // bounce
      }
  }
  return 0;
}

static int arkanoid_ball (blob* player, blob* wall, blob* ball, blob* box, char* mask, int* directionX, int* directionY) {
  int state = 0;
  unsigned int k;

  /// check timer has elapsed
  if (timer_get (0) <= 0) {
    blob ball_old;

    ball_old.xs = ball->xs;
    ball_old.ys = ball->ys;
    ball_old.xe = ball->xe;
    ball_old.ye = ball->ye;

    // Compute new Ball Coordinates in current Frame
    ball->xs += *directionX;
    ball->xe += *directionX;
    ball->ys += *directionY;
    ball->ye += *directionY;

    if (ball->xe > Xmax) {
      ball->xe = Xmax;
      ball->xs = Xmax - 2;
    }
    else if (ball->xs < 0) {
      ball->xs = 0;
      ball->xe = 2;
    }
    if (ball->ys < 0) {
      ball->ys = 0;
      ball->ye = 2;
    }

    // Check Collision of Ball with other Objects
    if (arkanoid_bounce (player, ball)) {
      int diff = (player->xe - player->xs) >> 3;
      if ((ball->xs < player->xs + diff) || (ball->xs > player->xe - diff)) {
          *directionY = -3;
          if (*directionX > 0)
              *directionX = 1;
          else
              *directionX = -1;
      }
      else
          *directionY = -2;

      ball->ys += *directionY;
      ball->ye += *directionY;
      ball->xs -= *directionX;
      ball->xe -= *directionX;
    }
    // Ball below level of player -> Game Over
    else if (ball->ys > player->ye)
      state = 1;
    // Other situations
    else {
      int broke = 0; // Front: 1, Back: 2, Left: 3, Right: 4
      unsigned int empty_px = 0;

      // Check Collision of ball with walls
      for (k = 0; k < 3; k++) {
        if (arkanoid_bounce (&wall[k], ball)) {
          if (wall[k].xs == wall[k].xe) {
            ball->ys -= *directionY;
            ball->ye -= *directionY;

            if (*directionX > 0)
                *directionX = -2;
            else
                *directionX = 2;
            if (*directionY > 0)
                *directionY = 2;
            else
                *directionY = -2;

            ball->xs += *directionX ;
            ball->xe += *directionX;
          }
          else if (wall[k].ys == wall[k].ye) {
            ball->xs -= *directionX;
            ball->xe -= *directionX;

            if (*directionY > 0)
                *directionY = -2;
            else
                *directionY = 2;
            if (*directionX > 0)
                *directionX = 2;
            else
                *directionX = -2;

            ball->ys += *directionY;
            ball->ye += *directionY;
          }
          break;
        }
      }
      // Check Collision with Box
      for (k = 0; k < N; k++) {
        int j;
        int l;

        if (mask[k] == '0') {
          empty_px++;
          continue;
        }

        if (broke > -1) {
          for (j = ball->xs; j <= ball->xe; j++) {
            for (l = ball->ys; l <= ball->ye; l++) {
              if (j == box[k].xs && l == box[k].ys) {
                // Ball Front Side: (Xs<->Xe, Ys)
                // Ball Back Side: (Xs<->Xe, Ye)
                _DrawLine (box[k].xs, box[k].ys, box[k].xs, box[k].ys, _OFF_);
                mask[k] = '0';
                broke = 1;
                break;
              }
            }
          }
        }
        if (broke < 1) {
          for (j = ball->ys; j <= ball->ye; j++) {
            for (l = ball->xs; l <= ball->xe; l++) {
              if (j == box[k].ys && l == box[k].xs) {
                // Ball Left Side: (Xs, Ys<->Ye)
                // Ball Right Side: (Xe, Ys<->Ye)
                _DrawLine (box[k].xs, box[k].ys, box[k].xs, box[k].ys, _OFF_);
                mask[k] = '0';
                broke = -1;
                break;
              }
            }
          }
        }
      }

      // Broke Box with Front/Back Side
      if (broke == -1) {
        ball->ys -= *directionY;
        ball->ye -= *directionY;

        if (*directionX > 0)
          *directionX = -2;
        else
          *directionX = 2;
        if (*directionY > 0)
          *directionY = 2;
        else
          *directionY = -2;

        ball->xs += *directionX;
        ball->xe += *directionX;
      }
      // Broke Box with Left/Right side
      else if (broke == 1) {
        ball->xs -= *directionX;
        ball->xe -= *directionX;

        if (*directionY > 0)
          *directionY = -2;
        else
          *directionY = 2;
        if (*directionX > 0)
          *directionX = 2;
        else
          *directionX = -2;

        ball->ys += *directionY;
        ball->ye += *directionY;
      }

      // Whole Box was broken
      if (empty_px == N)
        state = -1;
    }
    _DrawRect (ball_old.xs, ball_old.ys, ball_old.xe, ball_old.ye, 0, _OFF_, _OFF_);
    DrawRect (ball->xs, ball->ys, ball->xe, ball->ye, 1, _ON_, _ON_);
    // Reset Frame timer
    timer_stop (0);
    timer_start (0, arkanoid_get_difficulty ());
  }

  return state;
}

static int arkanoid_loop (blob* player, blob* wall, blob* ball, blob* box, char* mask, int* directionX, int* directionY) {
  for (;;) {
    unsigned int idx;
    int r = arkanoid_ball (player, wall, ball, box, mask, directionX, directionY);

    // Game Over OR Victory
    if (r)
    {
      char title[17] = {0};

      strcpy (title, (r == 1) ? "GAME OVER!" : "YOU WON");
      display_medium (2, 5, title);
      ttestall (0, 100);

      switch (show_menu (title, 1, 0, NUMBER_OF_ITEMS(game_end), game_end)) {
        case 0: // Restart
          return 1;
        case 1: // Quit
        default:
          return 0;
      }
    }

    // Key pressed
    if ( ttestall (KEYBOARD, 1) & KEYBOARD ) {
      reset_buf (hKeyboard, _receive_id);
      char pressed_key = getchar ();

      switch (pressed_key) {
        // Cancel
        case T_ANN:
          _clrscr();
          return 0;

        // Pause
        case T_CORR: {
          display_medium (2, 5, "PAUSE");
          ttestall (0, 100);

          switch (show_menu ("PAUSE", 1, 0, NUMBER_OF_ITEMS(game_pause), game_pause)) {
            case 0: // Continue
              break;
            case 1: { // Difficulty
              int ret = show_menu ("DIFFICULTY", 1, 0, NUMBER_OF_ITEMS(game_difficulty), game_difficulty);

              if (ret > -1 && ret < 6)
                arkanoid_set_difficulty (ret);

              break;
            }

            case 2: // Restart
              return 1;

            case 3: // Exit
              return 0;
          }

          _clrscr();
          DrawLine (wall[0].xs, wall[0].ys, wall[0].xe, wall[0].ye, _ON_);
          DrawLine (wall[1].xs, wall[1].ys, wall[1].xe, wall[1].ye, _ON_);
          DrawLine (wall[2].xs, wall[2].ys, wall[2].xe, wall[2].ye, _ON_);
          DrawRect (player->xs, player->ys, player->xe, player->ye, 1, _ON_, _ON_);
          DrawRect (ball->xs, ball->ys, ball->xe, ball->ye, 1, _ON_, _ON_);
          for (idx = 0; idx < N; idx++)
            if (mask[idx] == '1')
              _DrawLine (box[idx].xs, box[idx].ys, box[idx].xs, box[idx].ys, _ON_);

          break;
        }

        case T_NUM4: // Left
          if (player->xs - STEP <= Xmin)
              break;

          player->xs -= STEP;
          player->xe -= STEP;

          _DrawRect (player->xs+STEP, player->ys, player->xe+STEP, player->ye, 0, _OFF_, _OFF_);
          DrawRect (player->xs, player->ys, player->xe, player->ye, 1, _ON_, _ON_);

          break;

        case T_NUM6: // Right
          if (player->xe + STEP >= Xmax)
              break;

          player->xs += STEP;
          player->xe += STEP;

          _DrawRect (player->xs-STEP, player->ys, player->xe-STEP, player->ye, 0, _OFF_, _OFF_);
          DrawRect (player->xs, player->ys, player->xe, player->ye, 1, _ON_, _ON_);

          break;

        default:
          break;
      }
    }
  }
}

void arkanoid_menu (void) {
  for (;;) {
    blob player = { 57, 62, 78, 63 };
    blob wall[3] = {
        { Xmin, Ymin, Xmax, Ymin }, // Wall, Up
        { Xmin, Ymin, Xmin, Ymax }, // Left Wall
        { Xmax, Ymin, Xmax, Ymax }  // Right Wall
    };
    blob ball = { 32, 14, 34, 16 };
    int directionX = 2, directionY = 2;
    blob box[N];
    char mask[N];
    unsigned int idx;

    for (idx = 0; idx < N; idx++) {
      box[idx].xs = 40 + (idx % 40);
      box[idx].ys = 10 + idx / 40;
      box[idx].xe = box[idx].ye = 0;
      mask[idx] = '1';
    }

    _clrscr();
    _DrawLine (wall[0].xs, wall[0].ys, wall[0].xe, wall[0].ye, _ON_);
    _DrawLine (wall[1].xs, wall[1].ys, wall[1].xe, wall[1].ye, _ON_);
    _DrawLine (wall[2].xs, wall[2].ys, wall[2].xe, wall[2].ye, _ON_);
    _DrawRect (player.xs, player.ys, player.xe, player.ye, 1, _ON_, _ON_);
    _DrawRect (ball.xs, ball.ys, ball.xe, ball.ye, 1, _ON_, _ON_);
    for (idx = 0; idx < N; idx++)
      if (mask[idx] == '1')
        _DrawLine (box[idx].xs, box[idx].ys, box[idx].xs, box[idx].ys, _ON_);

    timer_start (0, arkanoid_get_difficulty ());

    if (arkanoid_loop (&player, wall, &ball, box, mask, &directionX, &directionY) == 0)
      break;
  }

  timer_stop (0);
  arkanoid_set_difficulty (2);
}

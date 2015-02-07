

#ifndef _LINUX_ATARIKB_H
#define _LINUX_ATARIKB_H

void ikbd_write(const char *, int);
void ikbd_mouse_button_action(int mode);
void ikbd_mouse_rel_pos(void);
void ikbd_mouse_abs_pos(int xmax, int ymax);
void ikbd_mouse_kbd_mode(int dx, int dy);
void ikbd_mouse_thresh(int x, int y);
void ikbd_mouse_scale(int x, int y);
void ikbd_mouse_pos_get(int *x, int *y);
void ikbd_mouse_pos_set(int x, int y);
void ikbd_mouse_y0_bot(void);
void ikbd_mouse_y0_top(void);
void ikbd_mouse_disable(void);
void ikbd_joystick_event_on(void);
void ikbd_joystick_event_off(void);
void ikbd_joystick_get_state(void);
void ikbd_joystick_disable(void);

/* Hook for MIDI serial driver */
extern void (*atari_MIDI_interrupt_hook) (void);
/* Hook for mouse driver */
extern void (*atari_mouse_interrupt_hook) (char *);
/* Hook for keyboard inputdev  driver */
extern void (*atari_input_keyboard_interrupt_hook) (unsigned char, char);
/* Hook for mouse inputdev  driver */
extern void (*atari_input_mouse_interrupt_hook) (char *);

int atari_keyb_init(void);

#endif /* _LINUX_ATARIKB_H */

#!/usr/bin/perl

use warnings;
use strict;
use Tk;

my $template_string = 'const unsigned char PROGMEM chr_%NUM%[] = {%WIDTH%,%PIXELS%}; // %NAME%';

my $window = MainWindow->new();
$window->title('Draw a font helper');

my $title_label = $window->Label(-text => 'Draw-a-Font')->pack(-side => 'top', -pady => 10);

my $drawing_frame = $window->Frame()->pack(-side => 'top', -pady => 10);

my $pixel_buttons;
my $pixels;
for my $y (0..7) {
	for my $x (0..10) {
		$pixel_buttons->[$x]->[$y] = $drawing_frame->Button(-background => 'black', -command => sub {
				$pixels->[$x]->[$y] = !$pixels->[$x]->[$y];
				if ($pixels->[$x]->[$y]) {
					$pixel_buttons->[$x]->[$y]->configure( -background => 'white');
				} else {
					$pixel_buttons->[$x]->[$y]->configure( -background => 'black');
				}
				UpdatePixels();
			})->grid(-row => (7-$y), -column => $x);
	}
}

my $definition_frame = $window->Frame()->pack(-side => 'top', -pady => 10);
my $char_name = 'A';
my $chr_num = '065';

my $char_field = $definition_frame->Entry(-textvariable => \$char_name, -width=> 20, -validate => 'all', -validatecommand => \&UpdatePixels)->grid(-row => 0, -column => 0, -padx => 20);
my $chr_num_field = $definition_frame->Entry(-textvariable => \$chr_num, -width=> 20, -validate => 'all', -validatecommand => \&UpdatePixels)->grid(-row => 0, -column => 1, -padx => 20);

my $current_string_frame = $window->Frame()->pack(-side => 'top', -pady => 10);


my $current_string;
UpdatePixels();

my $current_string_field = $current_string_frame->Entry(-textvariable => \$current_string, -width=> 200)->grid(-row => 0, -column => 0, -padx => 20);

my $buttons_frame = $window->Frame()->pack(-side => 'top', -pady => 10);

my $to_file_button = $buttons_frame->Button(-text => 'Add line to file "font.txt"', -command => \&AddToFile)->grid(-row => 0, -column => 0);
my $to_file_incr_button = $buttons_frame->Button(-text => 'Add line to file "font.txt" and increment', -command => \&AddToFile_Incr)->grid(-row => 0, -column => 1);
my $clear_button = $buttons_frame->Button(-text => 'Clear Canvas', -command => \&ClearCanvas)->grid(-row => 0, -column => 2);



MainLoop();

sub ClearCanvas {
	for my $y (0..7) {
		for my $x (0..10) {
			$pixel_buttons->[$x]->[$y]->configure( -background => 'black');
			$pixels->[$x]->[$y] = 0;
		}
	}
}

sub AddToFile_Incr {
	AddToFile();
	$char_name++;
	$chr_num++;
	$chr_num = sprintf("%03i", $chr_num);
}

sub AddToFile {
	open(FONTFILE, '>>', 'font.txt');
	print FONTFILE $current_string."\n";
	close(FONTFILE);
}

sub UpdatePixels {
	# get width
	my $width = 10;
	for my $x (reverse 0..10) {
		my $empty = 1;
		for my $y (0..7) {
			if ($pixels->[$x]->[$y]) {
				$empty = 0;
			}
		}
		if ($empty) {
			$width--;
		} else {
			last;
		}
	}
	my $row_values;
	for my $x (0..$width) {
		my $cur_value;
		for my $y (0..7) {
			if ($pixels->[$x]->[$y]) {
				$cur_value += 2**$y;
			}
		}
		$row_values->[$x] = $cur_value;
	}

	my $pixels_str;
	if ($row_values) {
		$pixels_str = join(',',map {sprintf("0x%02X", $_)} @$row_values);
	} else {
		$pixels_str = '';
	}
	$width++;
	$width = sprintf("0x%02X", $width);

	$current_string = $template_string;
	$current_string =~ s/\%NUM\%/$chr_num/;
	$current_string =~ s/\%WIDTH\%/$width/;
	$current_string =~ s/\%PIXELS\%/$pixels_str/;
	$current_string =~ s/\%NAME\%/$char_name/;

	return 1;
}

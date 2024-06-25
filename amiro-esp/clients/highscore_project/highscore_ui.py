from highscore_esp_connection import ESP_AMiRo

from kivy.config import Config
Config.set('graphics', 'window_state', 'maximized')

from highscore_app import Highscores as scores
from kivy.core.window import Window
from kivy.metrics import dp
from kivymd.uix.screen import MDScreen
from kivymd.app import MDApp
from kivymd.uix.datatables import MDDataTable
from kivymd.uix.button import MDRaisedButton, MDTextButton
from kivymd.uix.tab import MDTabsBase, MDTabs
from kivymd.uix.floatlayout import MDFloatLayout
from kivymd.uix.slider import MDSlider
from kivymd.uix.textfield import MDTextField

red = [0.6, 0, 0, 0.8] 
green = [0, 0.5, 0, 0.8] 
blue = [0, 0, 0.6, 0.6] 
yellow = [0.9,0.7,0.3,0.8]
orange = [0.9,0.8,0.1,0.8]



class Tab(MDFloatLayout, MDTabsBase):
    '''Class implementing content for a tab.'''

class HighscoreTable(MDDataTable):
	'''Class implementing Table design and behaviour'''
	def sort_on_name(self, data):
		return zip(*sorted(enumerate(data), key=lambda l: l[1][1]))
	def sort_on_total(self, data):
		return zip(*sorted(enumerate(data), key=lambda l: l[1][2]))
	def sort_on_speed(self, data):
		return zip(*sorted(enumerate(data), key=lambda l: l[1][3]))
	def sort_on_accuracy(self, data):
		return zip(*sorted(enumerate(data), key=lambda l: l[1][4]))
	def sort_on_efficiency(self, data):
		return zip(*sorted(enumerate(data), key=lambda l: l[1][5]))
		
	def __init__(self, scores, table_width):
		super().__init__(
				pos_hint = {'center_x': 0.5, 'center_y': 0.5},
				size_hint = (0.9, 0.6),
				sorted_on = "Total Score",
				sorted_order = "ASC",
				use_pagination = False,
				rows_num = 100,
				column_data = [
					("[size=24]No.[/size]", dp(table_width*0.05)),
					("Name", dp(table_width*0.19), self.sort_on_name),
					("Total Score", dp(table_width*0.19), self.sort_on_total),
					("Speed", dp(table_width*0.19), self.sort_on_speed),
					("Accuracy", dp(table_width*0.19), self.sort_on_accuracy),
					("Efficiency", dp(table_width*0.19), self.sort_on_efficiency)
				],
				row_data = scores.df.to_records()
				)

class HighscoreApp(MDApp):
	'''Main GUI class as KivyMDApp'''

	def waiting_connect(self,button):
		'''Function for Button "Connect"'''
		button.text = "Waiting for connection to ESP"
		button.md_bg_color = yellow


	def connect(self, button):
		'''Function for Button "Connect"'''
		try:
			self.esp.amiro_ip = self.input_ip.text
			self.esp.connect()
			button.md_bg_color = green
			self.button_disconnect.md_bg_color = self.theme_cls.primary_color
			button.text = "Connected to ESP"
		except:
			button.md_bg_color = red
			button.text = "Failed to Connect -> Reset ESP?"
			return False

	
	def disconnect(self, button):
		'''Function for Button "Disconnect"'''
		self.esp.disconnect()
		button.md_bg_color = red
		self.button_connect.md_bg_color = self.theme_cls.primary_color
		self.button_connect.text = "Connect"

	
	def waiting(self,button):
		'''Function for Button "Listen" on press'''

		#weighting_switch: checks if the AMiRo has send the Weighting-Data over the esp
		if self.weighting_switch == 0:
			self.button_listen.md_bg_color = yellow
			self.button_listen.text = "Please hug the AMiRo (Sensor [1][2] & [5][6])"
		else:
			self.tabs.switch_tab("LineFollowing")
			#checks if user_name has been set
			try:
				if self.user_name == "":
					if self.judgment_data[0] == 1:
						self.user_name = "Formula_AMiRo"
						self.input_name.text = self.user_name
					else:
						self.user_name = "WALL-E"
						self.input_name.text = self.user_name
				pass
			except:
				if self.judgment_data[0] == 1:
					self.user_name = "Formula_AMiRo"
					self.input_name.text = self.user_name
				else:
					self.user_name = "WALL-E"
					self.input_name.text = self.user_name
			self.button_listen.md_bg_color = yellow
			self.button_listen.text = "Waiting for the AMiRo to finish Evaluation"

	# on button_release
	def listen_to_AMiRo(self,button):
		'''Functions for Button "Listen" on release'''

		#checks for esp data
		score = self.esp.got_weighting(self.weighting_switch)
		if self.weighting_switch == 0:
			# -> expects weighting_data
			if score == 5:
				self.button_listen.md_bg_color = red
				self.button_listen.text = "No Weighting -> Please Restart AMiRo and GUI"
			elif score == False:
				self.button_connect.md_bg_color = red
			# -> wall or linefollowing?
			elif (score[0] == 1 or score[0] == 2):
				self.button_listen.md_bg_color = green
				self.weighting_switch= 1
				self.judgment_data = score
				if self.judgment_data[0] == 1:
					self.tabs.switch_tab("LineFollowing")
				else:
					self.tabs.switch_tab("WallFollowing")
				self.receive_weighting(self.judgment_data[1:])
				self.button_listen.text = "Listen to Score"
		else:
			# expects score -> else:
			if (score == 7):
				self.button_listen.text = "Cheat Detected -> Press again to try on new lap"
				self.button_listen.md_bg_color =  self.theme_cls.primary_color
				return
			# prints new score in csv file
			elif self.judgment_data[0] == 1:
				self.tabs.switch_tab("LineFollowing")
				self.lf_scores.add_entry(str(self.user_name),score[1],score[2],score[3])
				self.lf_scores.sort_by_total()
				self.reload_table_data()
			elif self.judgment_data[0] == 2:
				self.tabs.switch_tab("WallFollowing")
				self.wf_scores.add_entry(str(self.user_name),score[1],score[2],score[3])
				self.wf_scores.sort_by_total()
				self.reload_table_data()
			self.button_listen.text = "Listen to new LapScore"
			self.button_listen.md_bg_color = green


	def on_enter_name(self, value):
		'''Function for Text_Input Widget'''
		if self.input_name.has_had_text:
			self.user_name = value.text
			self.input_name.error = False
		else:
			self.input_name.error = True

						
	def change_weighting(self, instance, value):
		'''Recalculate TotalScores for Weighting Slider Movements'''
		if instance == self.slider_speedWeighting:
			self.lf_scores.weightings[0] = value/100
			self.wf_scores.weightings[0] = value/100
		elif instance == self.slider_accuracyWeighting:
			self.lf_scores.weightings[1] = value/100
			self.wf_scores.weightings[1] = value/100
		elif instance == self.slider_efficiencyWeighting:
			self.lf_scores.weightings[2] = value/100
			self.wf_scores.weightings[2] = value/100
		self.lf_scores.set_allTotalScores()
		self.wf_scores.set_allTotalScores()
		self.reload_table_data()
					
	def receive_weighting(self, values):
		'''Handle received weighting data from AMiRo'''
		self.lf_scores.weightings[0] = values[0]
		self.wf_scores.weightings[0] = values[0]
		self.lf_scores.weightings[1] = values[1]
		self.wf_scores.weightings[1] = values[1]
		self.lf_scores.weightings[2] = values[2]
		self.wf_scores.weightings[2] = values[2]
		self.lf_scores.set_allTotalScores()
		self.wf_scores.set_allTotalScores()
		self.slider_speedWeighting.value = values[0]*100
		self.slider_accuracyWeighting.value = values[1]*100
		self.slider_efficiencyWeighting.value = values[2]*100
		self.reload_table_data()
	
	def reload_table_data(self, *args):
		self.lf_table.row_data = self.lf_scores.df.to_records()
		self.wf_table.row_data = self.wf_scores.df.to_records()

	def build_tables(self, window_width=1920, column_factor=0.177):
		table_width = window_width * column_factor
		self.lf_table = HighscoreTable(self.lf_scores, table_width)
		self.wf_table = HighscoreTable(self.wf_scores, table_width)

	def window_resize(self, *args):
		'''Re-Add table widgets'''
		self.lf_tab.remove_widget(self.lf_table)
		self.wf_tab.remove_widget(self.wf_table)

		self.build_tables(window_width=Window.size[0])

		self.lf_tab.add_widget(self.lf_table)
		self.wf_tab.add_widget(self.wf_table)

	# On request close event
	def close(self, *args):
		self.esp.disconnect()

	def build(self):
		'''Build the KivyMD UI'''
		
		# Load Highscore lists
		self.lf_scores = scores("linefollowing_scores.csv")
		self.wf_scores = scores("wallfollowing_scores.csv")

		# Define Theme
		self.theme_cls.theme_style = "Light"
		self.theme_cls.primary_palette = "BlueGray"
		
		# Window binds
		Window.bind(on_resize=self.window_resize)
		Window.bind(on_request_close=self.close)
		
		# Define Screen
		self.screen = MDScreen()
		
		# Define Tables
		self.build_tables()
		
		# Define Tabs
		self.tabs = MDTabs(tab_hint_x = True, allow_stretch = True, lock_swiping = True)
		self.lf_tab = Tab(title="LineFollowing")
		self.wf_tab = Tab(title="WallFollowing")

		# Weighting Sliders
		self.label_weighting = MDTextButton(text = "Current Weighting:",
												disabled = True,
												font_style = "H4",
												disabled_color = blue,
												text_color = blue,
												theme_text_color = 'Hint',
												pos_hint = {'center_x': 0.25, 'center_y': 0.8})
		self.slider_speedWeighting = MDSlider(size_hint = (0.13, None),
												pos_hint = {'center_x': 0.51, 'center_y': 0.8},
												range = (0, 100),
												step = 10,
												value = 100)
		self.slider_accuracyWeighting = MDSlider(size_hint = (0.13, None),
												pos_hint = {'center_x': 0.68, 'center_y': 0.8},
												range = (0, 100),
												step = 10,
												value = 100)
		self.slider_efficiencyWeighting = MDSlider(size_hint = (0.13, None),
												pos_hint = {'center_x': 0.85, 'center_y': 0.8},
												range = (0, 100),
												step = 10,
												value = 100)
		self.slider_speedWeighting.bind(value=self.change_weighting)
		self.slider_accuracyWeighting.bind(value=self.change_weighting)
		self.slider_efficiencyWeighting.bind(value=self.change_weighting)

		# Define Buttons
		self.button_connect = MDRaisedButton(size_hint=(None, 0.05),
								pos_hint={'center_x': 0.21, 'center_y': 0.1},
								text='Connect',
								on_press=self.waiting_connect,
								on_release=self.connect)
		self.button_disconnect = MDRaisedButton(size_hint=(None, 0.05),
									pos_hint={'center_x': 0.3, 'center_y': 0.1},
									text='Disconnect',
									on_press=self.disconnect)
		self.button_listen = MDRaisedButton(size_hint=(0.2, 0.05),
								pos_hint={'center_x': 0.8, 'center_y': 0.1},
								text='Listen To Weighting',
								on_press=self.waiting,
								on_release=self.listen_to_AMiRo)

		self.input_ip = MDTextField(hint_text = "AMiRo IP",
									text = self.esp.amiro_ip,
									required = True,
									size_hint = (0.1, 0.05),
									pos_hint = {'center_x': 0.1, 'center_y': 0.1})

		self.input_name = MDTextField(hint_text = "Name (press 'Enter')",
										required = True,
										error = True,
										helper_text_mode = "on_error",
										size_hint=(0.1, 0.05),
										pos_hint = {'center_x': 0.6, 'center_y': 0.1},)
		self.input_name.bind(on_text_validate = self.on_enter_name)

		# Add widgets
		self.screen.add_widget(self.tabs)
		self.tabs.add_widget(self.lf_tab)
		self.tabs.add_widget(self.wf_tab)
		self.lf_tab.add_widget(self.lf_table)
		self.wf_tab.add_widget(self.wf_table)
		self.screen.add_widget(self.label_weighting)
		self.screen.add_widget(self.slider_speedWeighting)
		self.screen.add_widget(self.slider_accuracyWeighting)
		self.screen.add_widget(self.slider_efficiencyWeighting)
		self.screen.add_widget(self.input_ip)
		self.screen.add_widget(self.button_connect)
		self.screen.add_widget(self.button_disconnect)
		self.screen.add_widget(self.button_listen)
		self.screen.add_widget(self.input_name)

		return self.screen


	def __init__(self, esp: ESP_AMiRo):
		"""ESP Instanz festlegen
		
		Parameter
		---------
		esp : esp_connect
			Instanz der ESP Steuerungsklasse
		"""
		self.weighting_switch = 0
		self.esp = esp
		MDApp.__init__(self)



# HighscoreApp().run()
import os
import re
import ffmpeg


# 入出力ファイルのディレクトリ
INPUT_DIR = './input'
OUTPUT_DIR = './output'

# 入出力フォーマット
INPUT_FORMAT = '.wav'
OUTPUT_FORMAT = '.ac3'

# 必要なファイルのファイル名一覧
FILE_LIST = ['C', 'L', 'R', 'LFE', 'Ls', 'Rs']


files = os.listdir(INPUT_DIR)

# 拡張子を削除したファイル名リスト
files_name = [re.sub('\\' + INPUT_FORMAT + '$', '', file) for file in files]

# 必要なファイルが全て存在するか確認
if set(FILE_LIST) == set(files_name):

  # 音源リスト
  audio = []

  for name in FILE_LIST:
    audio.append(ffmpeg.input(INPUT_DIR + '/' + name + INPUT_FORMAT))

  # 音源変換
  (
    ffmpeg
    .filter(audio, 'join', inputs = 6, channel_layout = '5.1(side)')
    .output(OUTPUT_DIR + '/audio' + OUTPUT_FORMAT, format = re.sub('^.', '', OUTPUT_FORMAT))
    .run()
  )

else:
  print('[Error] All audio files not found.')

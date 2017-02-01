 LoopMID2LoopOGG
=================

本ソフトは、「RPGツクール2000規格で作成された、対応するRPGツクール(2000,2003,XP,VX,VXAce 等)においてループする標準MIDIファイルを、対応するRPGツクール(VXAce,MV 等)においてループするOGG Vorbisファイルへ変換する」ツールです。

This software is a converter from looping Standard MIDI File which is conformable to RPG Maker 2000 to looping OGG Vorbis File by special tag LOOPSTART/LOOPLENGTH. (I'm not good at English, so contribution of translation is highly appreciated. Please see Contribution in this README.)

** 本ソフトを入手、利用した時点で、本ソフトの構成要素に付されている全てのライセンス条項にあなたが同意し、README(本文書)の「ツール利用上の注意事項」を理解し同意したものと見なします。 ** ライセンスについては同梱の LICENSE をご参照下さい。

プロジェクトページは [こちら](https://wl-amigo.github.io/LoopMID2LoopOGG/) です。


## 何が出来るの？

* RPGツクール2000規格で作成されたいわゆるMIDI素材(CC111を用いてループするように作られたMIDI素材)を、LOOPSTART/LOOPLENGTHタグによるループ情報を埋め込んだOGG Vorbisファイルへ変換することが出来ます。
* 変換する前に、変換結果を確認するためのプレビュー再生を簡単に行うことが出来ます(プレビュー再生ではループ再生はされません)。
* 複数のMIDI素材をまとめて変換することもできます。
* 他の形式への変換用にWAV出力もできます。
* ループ再生に不要なデータを削って出力する「サイズ節約モード」と、音楽プレイヤーで聴くために指定回数ループとフェードアウトを行って出力する「サウンドトラックモード」が使えます。
* 変換時に同時にループ情報(LOOPSTART/LOOPLENGTH)を書きだしたjsonファイルも得られます。テキストエディタで開けばサンプル数単位のループ情報を得られます。


## セットアップ

1. [ここ](https://github.com/WL-Amigo/LoopMID2LoopOGG/releases) からZIPパッケージをダウンロードします。
2. 音源データ(サウンドフォント)をダウンロードします。今の所、SGM V2.01のみサポートされていますので、[ここ](https://sourceforge.net/projects/androidframe/files/soundfonts/) にアクセスして、"SGM-V2.01.sf2" を選択し、ダウンロードして下さい(サイズが大きいため時間が掛かります)。
3. 1番でダウンロードしたZIPパッケージを、適当な場所に展開します。
4. 2番でダウンロードした音源データ(SGM-V2.01.sf2)を、3番で展開したパッケージ内の "TiMidity++/sf2/" フォルダに移動します。
5. これで準備完了です。 "LoopMID2LoopOGG.exe" をダブルクリックすると起動できます。


## 使い方

1. まず、出力先のフォルダを `Output Directory` に設定します。設定しないと変換できません。
2. 変換したいMIDIファイルをウィンドウ中央のアイコン部分( Convert と書かれている部分)にドラッグ＆ドロップします。
3. 出てきたダイアログの「変換中…」が「変換完了！」に変わったら変換完了です。変換済みのOGGファイルは1番で指定したフォルダに出力されます。

変換済みOGGファイルには自動的にループタグが埋め込まれるので、対応するRPGツクールなどの環境で継ぎ目なくループ再生されます。

もしループ再生されるかをいちいちRPGツクールでインポートして再生…とするのが面倒な方は、同梱している "OggLoopPlayer.exe" をご利用下さい。OggLoopPlayerの利用方法は以下のとおりです。

1. ウィンドウ上部の "File" をクリックし、出てきたメニューから "Open" を選択します。
2. ファイル選択画面が出てきますので、ループタグが埋め込まれているOGGファイルを選択します。
3. 再生ボタン(三角マーク)を押します。
4. ウィンドウ上下中央のバーで大体の再生位置を指定し、右端の戻るボタン的なものを押すと、指定位置から再生することが出来ます

OggLoopPlayerは現在かなり不安定なので、もし再生ボタンをおしても再生できなくなったりしたらプレイヤーを再起動して下さい。ご迷惑をおかけします。


## 使用可能なオプションについて

### メイン画面

* 変換前にプレビュー再生する: この項目にチェックを入れると、変換する前にどのような結果になるか試聴することが出来ます。複数変換時には無視されます。

### 設定

`ファイル->設定` を選択すると設定画面を呼び出すことが出来ます。設定画面では下記の設定を変更できます。

#### 出力

* 出力ファイル形式: 出力ファイル形式を `OGG Vorbis (ループタグ付き)` と `WAVE形式` から選択できます。
* 出力モード
    * サイズ節約モード: ループ再生に必要のない波形データを削り、サイズを抑えて出力するモードです。 `ループ終了後に残す最大サンプル数` で設定されたサンプルだけ、ループ後に残します。(0にしても良いと思われますが、念の為入れてあります)
    * サウンドトラックモード: 音楽プレイヤーなどループ再生をサポートしないプレイヤーで聴きたい場合などのために、指定回数分のループとループ後のフェードアウトを出力に含めるモードです。 `ループ後からのフェードアウト開始秒数` は、ループ終了地点から何秒後にフェードアウトを開始するかを指定するオプションです。


## ツール利用上の注意事項

* 本ツールを使用して生成したOGGファイルに関する著作権は、変換元のMIDIファイルの著作権、及び変換に利用された音源データの著作権を全て受け継ぐものとします。
* 上記に従って、変換元のMIDIファイルの著作者、及び音源データの著作者が明示的に許可していない限り、生成されたOGGファイルの再配布等の著作権により保護される利用形態は許可されません。
* MIDIファイルの著作者が「他形式への変換しての素材利用」を明示的に許可していない場合、そのMIDIファイルに本ツールを適用し、素材として利用することは許可されません。
* MIDIファイルの著作者が明示的に制限している利用用途がある場合、本ツールを使用して生成したOGGファイルについても同様の制限を受けるものとします。
* 本ツールを利用したことによるトラブル、うまく変換できないなどの本ツールの動作にまつわるトラブル等について、MIDIファイルの著作者に問い合わせをしてはいけません。


### より具体的には？

* 変換元となるMIDI素材の制作者が「ゲームに同梱して使って良い」と表明している場合、本ツールを使用して変換したOGGファイルもゲームに同梱して使用して大丈夫です。
    * 但し、後述していますが「WAV/OGG等に変換して使っても良いです」と表明されていない場合は、本ツールを使用して生成したOGGファイルは使用できません。
* 本ツールを使用して生成したOGGファイルを、変換元のMIDI素材の制作者に許可無く、素材として再配布してはいけません。
* 本ツールを使用して生成したOGGファイルを自作したものと主張するのも絶対ダメです。
* もしMIDI素材の制作者が「WAV/OGG等に変換して使っても良いです」と明示的に書いていない場合は、問い合わせをして変換しても良いことを確認するまでは本ツールを使って変換してはいけません。
* MIDI素材の制作者が「こういう用途には使わないで下さい」と表明している場合、本ツールを使って変換したファイルも指定された用途に使用してはいけません。
* もし音源データの制作者が、その音源データの利用に関して何らかの制限を課している場合はそれに従わなければなりません。
    * 本ツールで利用(を推奨)している SGM-V2.01 は、[制作者により「フリーである」と表明されていて](http://dtmmuryo.seesaa.net/article/121970555.html)、かつ今までに既に様々な曲の制作に使われているので、問題は起こらないはずです。
* ただし、個人に閉じた利用(携帯音楽プレイヤーに入れて聴きたい、など)なら、著作権の法律上は問題ないので大丈夫です(再配布等は個人に閉じない利用なので依然としてダメです)。
* 「LoopMID2LoopOGGっていうツールを使ってあなたのMIDIが変換できない」のような問い合わせを、MIDIファイルの著作者にするのは絶対に止めてください。うまく変換できない場合の問い合わせはアミーゴ宛にお願いします。対応を検討します。
    * 「WAV/OGG等に変換して使っても良いか」は、本ツールに限定されないより一般的な問い合わせなのでしても良いと思いますが、十分にホームページ等に公開されている利用規約などを確認してからするようにして下さい。


## 動作確認済み環境

* Windows10, intel corei7, メモリ32GB
* Windows10, intel atom, メモリ2GB
* Ubuntu 14.04 LTS, intel corei7, メモリ8GB


## 現状の問題点

* 一部MIDI素材が正常に変換できないことが確認されています。正常に変換できないとは以下のような状況を指します。
    * 本来ループ開始するはずの地点で曲が途切れてしまう
    * ループ終了地点がズレていて無音時間が発生してしまう
* 音源データとして SGM-V2.01 しかサポートしていません。
* 楽器単位の音量調整が出来ないため、曲によってはあるパートが聴きづらくなります(例えばチェロなどが小さいです)
* 一部のMIDI素材において、ループ終了地点で楽器の発音が不自然に伸びることがあります。
* WAVへの出力はまだサポートしていません。
* 波形出力後の末尾処理を行っていないため、OGGファイルが無駄に大きいです(ループ再生時に再生されない波形を多く持っているためです)

上記の問題点については対策を検討中、もしくは対策は考案済みで実装を検討中です。


## 協力者募集！ (Contribution)

下記のような協力を激しく歓迎致します。

* 正常に変換できないMIDI素材の報告
    * 「配布元のホームページのアドレス」「曲名」を教えて頂けると、バグ修正のためのテストを多く行えるようになるため大変助かります。
* 改善パッチのプルリクエスト
    * 上記の現状の問題点の修正・改善や、何らかの改善を含むプルリクエストを歓迎致します。
* 翻訳 (translations)
    * I'm not good at using English, so I may produce strange translation if I try to translate this software's sentences. Translation from Japanese to your familiar language is highly appreciated. If you wonder how to send your translation, please contact to Amigo via issue on this repository, or Twitter, or E-Mail.
* 使い方などの記事の執筆
    * 使い方、応用方法など、本ソフトに関する記事(ブログ記事、Twitter モーメント等)の作成を歓迎致します。
    * 記事を作成して頂いた際、アミーゴ宛にご一報頂けるとすごく喜びます。
* 他の音源データへの対応
    * TiMidity++用のcfgファイルを作成できれば、本ソフトで使用できる音源データの拡張を行うことが出来ます。
* 本ツールの宣伝
    * もし本ツールを気にいって頂けましたら、是非Twitterやブログなどの上で紹介して下さい。本ツールを求めている方に届く可能性が高くなります。
    * Twitterにおけるハッシュタグは `#LoopMID2LoopOGG` です。


## 本ツールに関する問い合わせについて

本ツールは、Apache License Version 2.0に基づいて配布されるフリーソフトです。本ツールに関することについて、本ツールの制作者(協力者含む)は一切の責任を負いません。

…が、あくまで「対応するか分からないよ」というだけで、本ツールに関する問い合わせはして頂いても大丈夫です。その場合、以下の連絡先へ、LoopMID2LoopOGGの問い合わせであることを明記の上ご連絡ください。

Twitter: @WL_Amigo
E-Mail: white.luckers at gmail.com

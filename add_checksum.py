import os
import lpc_checksum

Import("env")

def add_lpc_checksum(source, target, env):
    # ファームウェアのバイナリファイルへのパスを取得します
    firmware_bin = os.path.join(env["PROJECT_BUILD_DIR"], env["PIOENV"], "firmware.bin")
    if os.path.exists(firmware_bin):
        print(f"Calculating and adding LPC checksum to {firmware_bin}")
        try:
            # チェックサムを計算してバイナリに直接書き込みます
            lpc_checksum.checksum(firmware_bin, read_only=False)
            print("LPC checksum added successfully.")
        except Exception as e:
            print(f"Failed to add LPC checksum: {e}")
    else:
        print(f"Warning: firmware.bin not found at {firmware_bin}")

# ビルド後のアクションとしてスクリプトを環境に追加します
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", add_lpc_checksum)
env.AddPostAction("$BUILD_DIR/firmware.bin", add_lpc_checksum) # セーフティネット


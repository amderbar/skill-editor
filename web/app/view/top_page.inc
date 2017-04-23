<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/index.css')?>">
    <title><?=APP_NAME?></title>
</head>
<body>
    <header class="with-btns">
        <ul id="system_menu" class="menu">
            <?php foreach ([
                    'ファイル',
                    '編集',
                    '検索',
                    'ツール',
                    '設定',
                    'ウィンドウ',
                    'ヘルプ'
            ] as $link_txt) { ?>
                <li><?= Html::link($link_txt, 'javascript:void(0);', [], ['class' => 'btn']) ?></li>
            <?php } ?>
        </ul>
    </header>

    <iframe id="main" name="main" src="main<?= isset($proj_id) ? '?pid='.$proj_id : '' ?>" scrolling="no" frameborder="no"></iframe>

    <footer><strong id="top-title"><?=APP_NAME?></strong> <?=VERSION?></footer>
</body>
</html>
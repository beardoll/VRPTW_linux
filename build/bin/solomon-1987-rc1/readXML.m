% ������ȡ����
list=dir(['C:\Users\beardollPC\Documents\DDVRPBTW\Solomon-Benchmark\solomon-1987-rc1\','*.xml']);
fileAmount = length(list);
for fileindex = 1:fileAmount
    % ��ȡxml�ļ�
    infilename =  list(fileindex).name;
    % infilename = 'RC101_025.xml';   % Ҫ��ȡ�ļ���
    try
        xDoc = xmlread(infilename);
    catch
        error('Failed to read XML file %s', infilename);
    end
    cx = []; % ���x����
    cy = []; % ���y����
    start_time = []; % ���ʱ�䴰��ʼʱ��
    end_time = [];   % ���ʱ�䴰����ʱ��
    quantity = [];   % ��Ż���������
    service_time = [];  % ��ŷ���ʱ��

    allXcoordItems = xDoc.getElementsByTagName('cx');  % x����
    allYcoordItems = xDoc.getElementsByTagName('cy');  % y����
    capacityNode = xDoc.getElementsByTagName('capacity');  % ������
    max_travel_timeNode = xDoc.getElementsByTagName('max_travel_time');  % �����������ʱ��
    allStarttimeItems = xDoc.getElementsByTagName('start');  % ʱ�䴰����ʼʱ��
    allEndtimeItems = xDoc.getElementsByTagName('end');  % ʱ�䴰����ֹʱ��
    allQuantityItems = xDoc.getElementsByTagName('quantity');  % ����������
    allServiceTimeItems = xDoc.getElementsByTagName('service_time');  % ����ʱ��

    % ��ȡ��������
    data = char(capacityNode.item(0).getFirstChild.getData);
    capacity = str2num(data);

    % ��ȡ�����������ʱ��
    data = char(max_travel_timeNode.item(0).getFirstChild.getData);
    max_travel_time = str2num(data);

    % ��ȡx����
    for i = 0 : allXcoordItems.getLength - 1
        data = char(allXcoordItems.item(i).getFirstChild.getData);
        if i == 0  % �ֿ�����
            depotx = str2num(data);
        else
            cx = [cx, str2num(data)];
        end
    end

    % ��ȡy����
    for i = 0 : allYcoordItems.getLength - 1
        data = char(allYcoordItems.item(i).getFirstChild.getData);
        if i == 0  % �ֿ�����
            depoty = str2num(data);
        else
            cy = [cy, str2num(data)];
        end
    end

    % ��ȡʱ�䴰��ʼʱ��
    for i = 0 : allStarttimeItems.getLength - 1
        data = char(allStarttimeItems.item(i).getFirstChild.getData);
        start_time = [start_time, str2num(data)];
    end

    % ��ȡʱ�䴰����ʱ��
    for i = 0 : allEndtimeItems.getLength - 1
        data = char(allEndtimeItems.item(i).getFirstChild.getData);
        end_time = [end_time, str2num(data)];
    end

    % ��ȡ�˿͵����������
    for i = 0 : allQuantityItems.getLength - 1
        data = char(allQuantityItems.item(i).getFirstChild.getData);
        quantity = [quantity, str2num(data)];
    end

    % ��ȡ�˿͵����ʱ��
    for i = 0 : allServiceTimeItems.getLength - 1
        data = char(allServiceTimeItems.item(i).getFirstChild.getData);
        service_time = [service_time, str2num(data)];
    end

    % ����Ϊ.mat��ʽ
    S = regexp(infilename, '\.', 'split');  % ȥ��.xml��׺
    savefilename = char(S(1)); % ȡ��xml�ļ���������Ϊmat�ļ�������
    save(strcat('C:\Users\beardollPC\Documents\DDVRPBTW\Solomon-Benchmark\solomon-mat\', savefilename), 'cx', 'cy', 'depotx', 'depoty', 'start_time', 'end_time', 'quantity', 'service_time', 'capacity', 'max_travel_time');
end





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ʾ������ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% allCoordinatesItems = xDoc.getElementsByTagName_r('node'); % ����������ݸ���ȡ����
% for i = 0 : allCoordinatesItems.getLength - 1 % ÿ��node������ȡ
%     thisItem = allCoordinatesItems(i);    % ��ǰnode
%     childNode = thisItem.getFirstChild;   % node�ĵ�һ���ӽڵ㣨x���꣩
%     k = 0;  % 0��ʾx���꣬1��ʾy����
%     while ~empty(childNode)
%         if k == 0
%             cx = [cx childNode.getFirstChild.getData];
%         else
%             cy = [cy childNode.getFirstChild.getData];
%         end
%         k = k + 1;
%         childNode = childNode.getNextSibling;  % node����һ���ӽڵ㣨y���꣩
%     end    
% end
% allRequestItems = xDoc.getElementsByTagName_r('request');  % ��ȡʱ�䴰��������Ϣ
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


